#!/usr/bin/env wrensh

class FS {
  static exists(f) {
    return !IO.Process(["ls", f]).test()
  }
}

var iserror = Fn.new { |c|
  return (c is Num && c != 0)
}

var sha256 = Fn.new { |val|
  var out = IO.run(["sh", "-c", "echo \"%(val)\" | sha256sum"])
  return out.split(" ")[0]
}

var argsquote = Fn.new { |args|
  var quoted = []
  for (arg in args) {
    arg.replace("\\", "\\\\")
    quoted.add("'%(arg)'")
  }
  return quoted.join(" ")
}

var read_args = Fn.new { |args|
  var nself = args[0].split(".")[0].endsWith("wrensh") ? 2 : 1

  var cmd_args = {}
  var pkg_args = []
  for (argi in (nself + 1)...args.count) {
    var arg = args[argi]
    if (arg == "--") {
      pkg_args = args[argi + 1..-1]
      break
    }
    if (arg.contains("=")) {
      var parts = arg.split("=")
      var key = parts[0]
      var value = parts[1..-1].join("=")
      cmd_args[key] = value
    } else {
      System.print("unrecognized argument %(arg), separate build script args with --")
      IO.exit(1)
    }
  }

  return {
    "self": args[0...nself],
    "pkg": args.count > nself ? args[nself] : null,
    "pkg_args": pkg_args,
    "pkg_args_str": argsquote.call(pkg_args),
    "args": cmd_args,
  }
}

class BuildCheck {
  construct new(self_args, outdir, pkgdir, devmode, host, target) {
    _self = self_args
    _outdir = outdir
    _pkgdir = pkgdir
    _devmode = devmode
    _host = host
    _target = target
    _need_build = null
    _build_reason = null
  }

  need_build { _need_build }
  build_reason { _build_reason }

  check() {
    _need_build = true

    if (!FS.exists("%(_outdir)/.ok")) {
      _build_reason = "not built"
      return
    }

    if (_devmode && FS.exists("%(_pkgdir)/dev_volatile")) {
      _build_reason = "volatile"
      return
    }

    if (FS.exists("%(_outdir)/dyndeps.txt") &&
        stale_("%(_outdir)/dyndeps.txt", _target)) {
      _build_reason = "dynamic dependencies stale"
      return
    }

    if (FS.exists("%(_outdir)/dyntooldeps.txt") &&
        stale_("%(_outdir)/dyntooldeps.txt", _host)) {
      _build_reason = "dynamic tool dependencies stale"
      return
    }

    _need_build = false
  }

  stale_(dyndeps_f, target) {
    var checks = []
    for (line in IO.run(["cat", dyndeps_f]).split("\n")) {
      if (line.isEmpty) continue
      var check = Fn.new {
        var fields = line.split("  ")
        var depid = fields[0]
        var depname = fields[1]
        var depargs = fields[2..-1]
        var env = IO.env()
        env["TARGET"] = target
        var depid2 = IO.run(_self + depargs, env).trim()
        return depid == depid2
      }
      checks.add(X.async(check))
    }
    for (val in X.await(checks)) {
      if (!val) return true
    }
    return false
  }
}

var getOpt = Fn.new { |opt|
  var optz = null
  if (["0", "Debug"].contains(opt)) {
    opt = 0
    optz = "Debug"
  } else if (["1", "Safe"].contains(opt)) {
    opt = 1
    optz = "ReleaseSafe"
  } else if (["2", "Fast"].contains(opt)) {
    opt = 2
    optz = "ReleaseFast"
  } else if (["Small"].contains(opt)) {
    opt = "s"
    optz = "ReleaseSmall"
  } else if (["3", "fast"].contains(opt)) {
    opt = opt
    optz = "ReleaseFast"
  } else if (["s", "z"].contains(opt)) {
    opt = opt
    optz = "ReleaseSmall"
  } else {
    System.print("unrecognized OPT=%(opt)")
    IO.exit(1)
  }
  return {
    "c": opt,
    "zig": optz,
  }
}

var getUserDevMode = Fn.new { |args, env|
  return (env["DEV"] == "1" || env["XOS_USER_DEV"] == "1" || args["args"]["DEV"] == "1")
}

var getHostTargetInfo = Fn.new { |host, target|
  // Host and target triples
  target = target || host

  var host_parts = host.split("-")
  var target_parts = target.split("-")

  return {
    "host": host,
    "target": target,
    "host_arch": host_parts[0],
    "host_os": host_parts[1].split(".")[0],
    "target_arch": target_parts[0],
    "target_os": target_parts[1].split(".")[0],
  }
}

var getDryMode = Fn.new { |args, env|
  return (env["XOS_DRY"] == "1" || args["args"]["DRY"] == "1")
}

var main = Fn.new {
  var full_timer = IO.Timer()

  var env = IO.env()
  var args = read_args.call(IO.args())

  var xos_root = env["XOS_ROOT"]
  var xos_id = env["XOS_ID"]
  var build_root = env["XOS_BUILD_ROOT"]
  var cache_root = "%(env["XOS_BUILD_ROOT"])/cache"
  var pkg_root = env["XOS_PKG_ROOT"]
  var pkg = args["pkg"]
  var build_pkg = "%(pkg_root)/%(pkg)"
  var dev_mode = getUserDevMode.call(args, env)
  var dry_mode = getDryMode.call(args, env)
  var target = getHostTargetInfo.call(
      env["HOST"],
      args["args"]["TARGET"] || env["TARGET"])
  var opt = getOpt.call(args["args"]["OPT"] || env["OPT"] || "s")

  if (!FS.exists("%(build_pkg)/build.sh")) {
    System.print("unrecognized pkg %(pkg), missing build.sh")
    IO.exit(1)
  }

  var pkgid_text = IO.run(["xos_internal_pkgid", pkg], {
    "PATH": env["PATH"],
    "XOS": xos_id,
    "BUILD_PKG": build_pkg,
    "TARGET": target["target"],
    "OPT": opt["c"],
    "ARGS": args["pkg_args_str"],
  })
  var pkgid = sha256.call(pkgid_text)

  var build_out = "%(cache_root)/pkg/%(pkgid[0...2])/%(pkgid)"

  if (!dev_mode && FS.exists("%(build_pkg)/dev_volatile")) {
    System.print("warn: %(pkg) marked volatile, but DEV != 1, consider unmarking")
  }

  // Do we need to build?
  var buildcheck = BuildCheck.new(
      args["self"],
      build_out,
      build_pkg,
      dev_mode,
      target["host"],
      target["target"])
  buildcheck.check()

  if (dry_mode) {
    var dry_log = "
  ---
  build %(pkg) %(args["pkg_args_str"])
  cached=%(buildcheck.need_build ? "n" : "y")
  build_reason=%(buildcheck.build_reason)

    BUILD_PKG=%(build_pkg)
    BUILD_OUT=%(build_out)

    TARGET=%(target["target"])
    HOST=%(target["host"])
    OPT=%(opt["c"])
    OPT_ZIG=%(opt["zig"])

    XOS_ROOT=%(xos_root)
    XOS_PKG_ROOT=%(pkg_root)
    XOS_BUILD_ROOT=%(build_root)
    XOS_ID=%(xos_id)
  ---
  "
    System.print(dry_log)
    IO.writeln(pkgid)
    IO.exit(0)
  }

  // build if necessary
  if (buildcheck.need_build) {
    IO.run(["rm", "-rf", build_out])
    IO.run(["mkdir", "-p", "%(build_out)/out", "%(build_out)/tmp"])
    IO.Process(["echo", pkgid_text]).stdout("%(build_out)/pkgid").run()

    // pkg-specific tools
    var pkg_tools_dir = "%(build_out)/tools"
    IO.run(["mkdir", "-p", pkg_tools_dir])
    var mktemp = "#!/usr/bin/env sh
  set -e
  xos_internal_mktemp \"$@\" \"%(build_out)/tmp/tmpXXXXXX\"
  "
    IO.Process(["echo", mktemp]).stdout("%(pkg_tools_dir)/mktemp").run()
    IO.run(["chmod", "+x", "%(pkg_tools_dir)/mktemp"])

    // setup log and interrupt/fail fns
    var logfile = "%(build_out)/build.log"

    var fail = Fn.new {
      IO.Process(["cat", logfile]).stdout(2).run()
      System.print("failed building pkg %(pkg) %(pkgid)")
      IO.exit(1)
    }

    var trap = IO.trap(2, Fn.new {
      IO.Process(["cat", logfile]).stdout(2).run()
      System.print("interrupted building pkg %(pkg) %(pkgid)")
      IO.exit(1)
    })

    // run build script
    var build_env = {
      // generic
      "PATH": "%(pkg_tools_dir):%(env["PATH"])",
      "HOME": "%(build_out)/tmp",
      "XDG_CACHE_HOME": "%(cache_root)/xdg",
      // xos internal use
      "XOS_ROOT": xos_root,
      "XOS_ID": xos_id,
      "XOS_BUILD_ROOT": build_root,
      "XOS_BUILD_CACHE": cache_root,
      "XOS_PKG_ROOT": pkg_root,
      "XOS_BUILD_OUT": build_out,
      "XOS_USER_DEV": dev_mode ? 1 : 0,
      "XOS_DRY": dry_mode ? 1 : 0,
      // for user use
      "HOST": target["host"],
      "HOST_OS": target["host_os"],
      "HOST_ARCH": target["host_arch"],
      "TARGET": target["target"],
      "TARGET_OS": target["target_os"],
      "TARGET_ARCH": target["target_arch"],
      "OPT": opt["c"],
      "OPT_ZIG": opt["zig"],
      "BUILD_OUT": "%(build_out)/out",
      "BUILD_DEPS": "%(build_out)/deps",
      "BUILD_TOOLS": "%(build_out)/tooldeps",
      "BUILD_PKG": "%(build_pkg)",
    }
    IO.chdir("%(build_out)/tmp")
    var script = "%(build_pkg)/build.sh"
    var out = IO.Process(["sh", "-e", script] + args["pkg_args"])
      .env(build_env)
      .stdout(logfile)
      .runc()
    trap.cancel()
    if (iserror.call(out)) fail.call()
  }

  IO.writeln(pkgid)

  var duration = full_timer.lap()
  var cache_str = buildcheck.need_build ? "" : " (cached)"
  System.print("%(pkg) built%(cache_str) in %(duration)ms")
}

main.call()
