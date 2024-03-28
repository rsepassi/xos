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

var parseArgs = Fn.new { |args|
  var nself = 2

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

    // set in check()
    _need_build = null
    _build_reason = null
  }

  need_build { _need_build }
  build_reason { _build_reason }

  check() {
    _need_build = true

    if (!_devmode && FS.exists("%(_pkgdir)/dev_volatile")) {
      System.print("warn: %(_pkgdir) marked volatile, but DEV != 1, consider unmarking")
    }

    if (!FS.exists("%(_outdir)/.ok")) {
      _build_reason = "not built"
      return
    }

    if (_devmode && FS.exists("%(_pkgdir)/dev_volatile")) {
      System.print("warn: %(_pkgdir) marked volatile, rebuilding")
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

      var same = check.call()
      if (!same) return true
    }
    return false
  }
}

var getOpt = Fn.new { |opt|

  var optModes = {
    "0": [0, "Debug"],
    "1": [1, "ReleaseSafe"],
    "2": [2, "ReleaseFast"],
    "3": [3, "ReleaseFast"],
    "s": ["s", "ReleaseSmall"],
    "z": ["z", "ReleaseSmall"],
  }

  var optZig = {
    "Debug": "0",
    "Safe": "1",
    "Fast": "2",
    "Small": "s",
  }

  opt = optZig[opt] || opt
  var opts = optModes[opt]

  if (opts == null) {
    System.print("unrecognized OPT=%(opt)")
    IO.exit(1)
  }

  return { "c": opts[0], "zig": opts[1] }
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

var finalLog = Fn.new { |pkg, duration, built|
  var cache_str = built ? "" : " (cached)"
  System.print("%(pkg) built%(cache_str) in %(duration)ms")
}

var Ctx = Data.Record("Ctx", [
  "self",
  "xos_root",
  "xos_id",
  "build_root",
  "cache_root",
  "pkg_root",
  "pkg",
  "pkg_srcdir",
  "dev_mode",
  "dry_mode",
  "target",
  "opt",
  "PATH",
  "pkg_args",
])

var getCtx = Fn.new { |args, env|
  var pkg = args["pkg"]
  var pkg_root = env["XOS_PKG_ROOT"]
  return Ctx.new({
    "self": args["self"],
    "xos_root": env["XOS_ROOT"],
    "xos_id": env["XOS_ID"],
    "build_root": env["XOS_BUILD_ROOT"],
    "cache_root": "%(env["XOS_BUILD_ROOT"])/cache",
    "pkg_root": pkg_root,
    "pkg": pkg,
    "pkg_srcdir": "%(pkg_root)/%(pkg)",
    "dev_mode": getUserDevMode.call(args, env),
    "dry_mode": getDryMode.call(args, env),
    "target": getHostTargetInfo.call(
      env["HOST"],
      args["args"]["TARGET"] || env["TARGET"]),
    "opt": getOpt.call(args["args"]["OPT"] || env["OPT"] || "s"),
    "PATH": env["PATH"],
    "pkg_args": args["pkg_args"],
  })
}

var Pkg = Data.Record("Pkg", [
  "id",
  "id_text",
  "srcdir",
  "outdir",
])

var getPkg = Fn.new { |ctx|
  if (!FS.exists("%(ctx.pkg_srcdir)/build.sh")) {
    Fiber.abort("unrecognized pkg %(ctx.pkg), missing build.sh")
  }

  var pkgid_text = IO.run(["xos_internal_pkgid", ctx.pkg], {
    "PATH": ctx.PATH,
    "XOS": ctx.xos_id,
    "BUILD_PKG": ctx.pkg_srcdir,
    "TARGET": ctx.target["target"],
    "OPT": ctx.opt["c"],
    "ARGS": argsquote.call(ctx.pkg_args),
  })
  var pkgid = sha256.call(pkgid_text)

  var build_out = "%(ctx.cache_root)/pkg/%(pkgid[0...2])/%(pkgid)"

  return Pkg.new({
    "id": pkgid,
    "id_text": pkgid_text,
    "srcdir": ctx.pkg_srcdir,
    "outdir": build_out,
  })
}

var dryMode = Fn.new { |ctx, pkg, buildcheck|
  var dry_log = "
  ---
  build %(ctx.pkg) %(ctx.pkg_args)
  cached=%(buildcheck.need_build ? "n" : "y")
  build_reason=%(buildcheck.build_reason)

    BUILD_PKG=%(pkg.srcdir)
    BUILD_OUT=%(pkg.outdir)

    TARGET=%(ctx.target["target"])
    HOST=%(ctx.target["host"])
    OPT=%(ctx.opt["c"])
    OPT_ZIG=%(ctx.opt["zig"])

    XOS_ROOT=%(ctx.xos_root)
    XOS_PKG_ROOT=%(ctx.pkg_root)
    XOS_BUILD_ROOT=%(ctx.build_root)
    XOS_ID=%(ctx.xos_id)
  ---
  "
  System.print(dry_log)
  IO.writeln(pkg.id)
}

var build = Fn.new { |ctx, pkg|
  IO.run(["rm", "-rf", pkg.outdir])
  IO.run(["mkdir", "-p", "%(pkg.outdir)/out", "%(pkg.outdir)/tmp"])
  IO.Process(["echo", pkg.id_text]).stdout("%(pkg.outdir)/pkgid").run()

  // pkg-specific tools
  var pkg_tools_dir = "%(pkg.outdir)/tools"
  IO.run(["mkdir", "-p", pkg_tools_dir])
  var mktemp = "#!/usr/bin/env sh
set -e
xos_internal_mktemp \"$@\" \"%(pkg.outdir)/tmp/tmpXXXXXX\"
"
  IO.Process(["echo", mktemp]).stdout("%(pkg_tools_dir)/mktemp").run()
  IO.run(["chmod", "+x", "%(pkg_tools_dir)/mktemp"])

  // setup log and interrupt/fail fns
  var logfile = "%(pkg.outdir)/build.log"

  var fail = Fn.new {
    IO.Process(["cat", logfile]).stdout(2).run()
    System.print("failed building pkg %(ctx.pkg) %(pkg.id)")
    IO.exit(1)
  }

  var trap = IO.trap(2, Fn.new {
    IO.Process(["cat", logfile]).stdout(2).run()
    System.print("interrupted building pkg %(ctx.pkg) %(pkg.id)")
    IO.exit(1)
  })

  // run build script
  var build_env = {
    // generic
    "PATH": "%(pkg_tools_dir):%(ctx.PATH)",
    "HOME": "%(pkg.outdir)/tmp",
    "XDG_CACHE_HOME": "%(ctx.cache_root)/xdg",
    // xos internal use
    "XOS_ROOT": ctx.xos_root,
    "XOS_ID": ctx.xos_id,
    "XOS_BUILD_ROOT": ctx.build_root,
    "XOS_BUILD_CACHE": ctx.cache_root,
    "XOS_PKG_ROOT": ctx.pkg_root,
    "XOS_BUILD_OUT": pkg.outdir,
    "XOS_USER_DEV": ctx.dev_mode ? 1 : 0,
    "XOS_DRY": ctx.dry_mode ? 1 : 0,
    // for user use
    "HOST": ctx.target["host"],
    "HOST_OS": ctx.target["host_os"],
    "HOST_ARCH": ctx.target["host_arch"],
    "TARGET": ctx.target["target"],
    "TARGET_OS": ctx.target["target_os"],
    "TARGET_ARCH": ctx.target["target_arch"],
    "OPT": ctx.opt["c"],
    "OPT_ZIG": ctx.opt["zig"],
    "BUILD_OUT": "%(pkg.outdir)/out",
    "BUILD_DEPS": "%(pkg.outdir)/deps",
    "BUILD_TOOLS": "%(pkg.outdir)/tooldeps",
    "BUILD_PKG": "%(pkg.srcdir)",
  }
  IO.chdir("%(pkg.outdir)/tmp")
  var script = "%(pkg.srcdir)/build.sh"
  var out = IO.Process(["sh", "-e", script] + ctx.pkg_args)
    .env(build_env)
    .stdout(logfile)
    .runc()
  trap.cancel()
  if (iserror.call(out)) fail.call()
}

var main = Fn.new {
  var timer = IO.Timer()

  var ctx = getCtx.call(
    parseArgs.call(IO.args()),
    IO.env())

  var pkg = getPkg.call(ctx)

  var buildcheck = BuildCheck.new(
      ctx.self,
      pkg.outdir,
      pkg.srcdir,
      ctx.dev_mode,
      ctx.target["host"],
      ctx.target["target"])
  buildcheck.check()

  if (ctx.dry_mode) return dryMode.call(ctx, pkg, buildcheck)

  if (buildcheck.need_build) build.call(ctx, pkg)
  IO.writeln(pkg.id)

  finalLog.call(ctx.pkg, timer.lap(), buildcheck.need_build)
}

main.call()
