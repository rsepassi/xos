#!/usr/bin/env wrensh

var DEBUG = IO.env("DEBUG") == "1"

var Gctx

class V {
  static log(s) {
    logn(s, 1)
  }
  static log2(s) {
    logn(s, 2)
  }
  static log3(s) {
    logn(s, 3)
  }

  static logn(s, n) {
    var v = Gctx.v
    if (v > 0 && v >= n) {
      System.print(s)
    }
  }
}

class FS {
  static exists(f) {
    return IO.Process(["ls", f]).test()
  }
}

var sha256 = Fn.new { |val|
  return IO.run(["sha256sum", "-c", val]).split(" ")[0]
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
      if (line.trim().isEmpty) continue
      var issame = Fn.new {
        var fields = line.split("  ")
        var depid = fields[0]
        var depname = fields[1]
        var pkgname = fields[2]
        var depargs = fields.count > 3 ? JSON.decode(fields[3]) : []
        var env = IO.env()
        env["TARGET"] = target
        var depid2 = IO.run(_self + [pkgname, "--"] + depargs, env).trim()
        return depid == depid2
      }

      if (!issame.call()) return true
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

  var host_abi = "none"
  if (host_parts.count > 2) {
    host_abi = host_parts[2].split(".")[0]
  }

  var target_abi = "none"
  if (target_parts.count > 2) {
    target_abi = target_parts[2].split(".")[0]
  }

  return {
    "host": host,
    "target": target,
    "host_arch": host_parts[0],
    "host_os": host_parts[1].split(".")[0],
    "host_abi": host_abi,
    "target_arch": target_parts[0],
    "target_os": target_parts[1].split(".")[0],
    "target_abi": target_abi,
  }
}

var getDryMode = Fn.new { |args, env|
  return (env["XOS_DRY"] == "1" || args["args"]["DRY"] == "1")
}

var getVerbosity = Fn.new { |args, env|
  var v = env["V"] || args["args"]["V"]
  if (v == null) return 0
  return Num.fromString(v)
}

var finalLog = Fn.new { |v, pkg, duration, built|
  if (v > 0) {
    var cache_str = built ? "" : " (cached)"
    var namepadding = 16 - pkg.name.count
    var name = pkg.name + " " * namepadding

    var durstr = "%(duration)ms%(cache_str)"
    var dursz = "xxxxxms (cached)".count
    var durpadding = dursz - durstr.count
    durstr = durstr + " " * durpadding

    System.print("%(name)\t%(durstr)\t%(pkg.id)")
  }
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
  "system_path",
  "system_home",
  "v",
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
    "system_path": env["XOS_SYSTEM_PATH"],
    "system_home": env["XOS_SYSTEM_HOME"],
    "v": getVerbosity.call(args, env),
  })
}

var Pkg = Data.Record("Pkg", [
  "name",
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
  V.log("building %(ctx.pkg) %(pkgid)")

  var build_out = "%(ctx.cache_root)/pkg/%(pkgid[0...2])/%(pkgid)"

  return Pkg.new({
    "name": ctx.pkg,
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
  IO.write("%(pkg.outdir)/pkgid", pkg.id_text)

  // pkg-specific tools
  var pkg_tools_dir = "%(pkg.outdir)/tools"
  IO.run(["mkdir", "-p", pkg_tools_dir])
  var mktemp = "#!/usr/bin/env sh
set -e
xos_internal_mktemp \"$@\" \"%(pkg.outdir)/tmp/tmpXXXXXX\"
"
  IO.write("%(pkg_tools_dir)/mktemp", mktemp)
  IO.run(["chmod", "+x", "%(pkg_tools_dir)/mktemp"])

  // setup log and interrupt/fail fns
  var logfile = DEBUG ? 2 : "%(pkg.outdir)/build.log"

  var fail = Fn.new {
    if (!DEBUG) IO.Process(["cat", logfile]).stdout(2).run()
    System.print("failed building pkg %(ctx.pkg) %(pkg.id)")
    IO.exit(1)
  }

  var trap = IO.trap(2, Fn.new {
    if (!DEBUG) IO.Process(["cat", logfile]).stdout(2).run()
    System.print("interrupted building pkg %(ctx.pkg) %(pkg.id)")
    IO.exit(1)
  })

  var home = "%(pkg.outdir)/tmp"

  // run build script
  var build_env = {
    "DEBUG": DEBUG ? 1 : 0,
    // generic
    "PATH": "%(pkg_tools_dir):%(ctx.PATH)",
    "HOME": home,
    "XDG_CACHE_HOME": "%(ctx.cache_root)/xdg",
    "V": ctx.v,
    // xos internal use
    "XOS_ROOT": ctx.xos_root,
    "XOS_ID": ctx.xos_id,
    "XOS_BUILD_ROOT": ctx.build_root,
    "XOS_BUILD_CACHE": ctx.cache_root,
    "XOS_PKG_ROOT": ctx.pkg_root,
    "XOS_BUILD_OUT": pkg.outdir,
    "XOS_USER_DEV": ctx.dev_mode ? 1 : 0,
    "XOS_DRY": ctx.dry_mode ? 1 : 0,
    "XOS_SYSTEM_PATH": ctx.system_path,
    "XOS_SYSTEM_HOME": ctx.system_home,
    // for user use
    "HOST": ctx.target["host"],
    "HOST_OS": ctx.target["host_os"],
    "HOST_ARCH": ctx.target["host_arch"],
    "HOST_ABI": ctx.target["host_abi"],
    "TARGET": ctx.target["target"],
    "TARGET_OS": ctx.target["target_os"],
    "TARGET_ARCH": ctx.target["target_arch"],
    "TARGET_ABI": ctx.target["target_abi"],
    "OPT": ctx.opt["c"],
    "OPT_ZIG": ctx.opt["zig"],
    "BUILD_OUT": "%(pkg.outdir)/out",
    "BUILD_DEPS": "%(pkg.outdir)/deps",
    "BUILD_TOOLS": "%(pkg.outdir)/tooldeps",
    "BUILD_PKG": "%(pkg.srcdir)",
  }
  IO.chdir("%(pkg.outdir)/tmp")
  var script = "%(pkg.srcdir)/build.sh"
  V.log("running build script %(script)")
  V.log3(build_env)
  var out = IO.Process(["sh", DEBUG ? "-ex" : "-e", script] + ctx.pkg_args)
    .env(build_env)
    .stdout(logfile)
    .runc()
  V.log("build script exit code %(out)")
  if (!DEBUG) {
    IO.run(["rm", "-rf", home, pkg_tools_dir])
  }
  trap.cancel()
  if (out != 0) fail.call()
}

var main = Fn.new {
  var timer = IO.Timer()

  var ctx = getCtx.call(
    parseArgs.call(IO.args()),
    IO.env())
  Gctx = ctx
  V.log3(ctx)

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

  finalLog.call(ctx.v, pkg, timer.lap(), buildcheck.need_build)
}

main.call()
