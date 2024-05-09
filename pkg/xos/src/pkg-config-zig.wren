var parse_pkg_str = Fn.new { |arg|
    var eq = arg.split("=")
    var hasmodule = eq.count > 1

    var slash = eq[-1].split("/")
    var haslib = slash.count > 1

    var lib = slash[-1]

    return {
      "lib": lib,
      "pkg": haslib ? slash[0] : lib,
      "module": hasmodule ? eq[0] : lib,
    }
}

var parse_pc_file = Fn.new { |pc_path, pkgparse, depsdir, deps_depsdir|
  var info = {}
  for (line in IO.read(pc_path).split("\n")) {
    if (line.startsWith("Libs:")) {
      line = line[5..-1].trim()
      info["libs"] = line
    } else if (line.startsWith("Cflags:")) {
      line = line[7..-1].trim()
      info["cflags"] = line
    } else if (line.startsWith("ZRoot:")) {
      info["zig_root"] = line[6..-1].trim()
    } else if (line.startsWith("ZRequires:")) {
      info["zig_deps"] = []
      for (dep in line[10..-1].trim().split(" ")) {
        var p = parse_pkg_str.call(dep)
        p["pc"] = "%(deps_depsdir)/%(p["pkg"])/zig/%(p["lib"]).pc"
        p["pc"] = IO.run(["realpath", p["pc"]]).trim()
        info["zig_deps"].add(p)
      }
    } else if (line.startsWith("ZLocalRequires:")) {
      info["zig_local_deps"] = []
      for (dep in line[15..-1].trim().split(" ")) {
        var p = parse_pkg_str.call(dep)
        p["pkg"] = pkgparse["pkg"]
        p["pc"] = "%(depsdir)/%(p["pkg"])/zig/%(p["lib"]).pc"
        p["pc"] = IO.run(["realpath", p["pc"]]).trim()
        info["zig_local_deps"].add(p)
      }
    }
  }
  return info
}

var All_modules = {}

var Emit_module_args = Fn.new { |pkgparse, depsdir, module_prefix|
  var lib = pkgparse["lib"]
  var pkg = pkgparse["pkg"]
  var module = pkgparse["module"]

  var pc_path = pkgparse["pc"] || IO.run(["realpath", "%(depsdir)/%(pkg)/zig/%(lib).pc"]).trim()
  if (All_modules.containsKey(pc_path)) {
    return
  }

  var rootdir = "%(depsdir)/%(pkg)"
  var dep_depsdir = IO.run(["realpath", "%(rootdir)/../deps"]).trim()

  var info = parse_pc_file.call(pc_path, pkgparse, depsdir, dep_depsdir)

  var module_full = "%(module_prefix)%(module)"

  if (info.containsKey("libs")) {
    var libs = info["libs"].replace("${rootdir}", rootdir)
    IO.write(" %(libs)")
  }

  if (info.containsKey("cflags")) {
    var cflags = info["cflags"].replace("${rootdir}", rootdir)
    IO.write(" %(cflags)")
  }

  for (dep in info["zig_local_deps"] || []) {
    var depname = All_modules[dep["pc"]] || "%(module_full)_%(dep["module"])"
    IO.write(" --dep %(dep["module"])=%(depname)")
  }

  for (dep in info["zig_deps"] || []) {
    var depname = All_modules[dep["pc"]] || "%(module_full)__%(dep["pkg"])_%(dep["module"])"
    IO.write(" --dep %(dep["module"])=%(depname)")
  }

  var rootfile = info["zig_root"].replace("${rootdir}", rootdir)
  IO.write(" -M%(module_full)=%(rootfile)")
  All_modules[pc_path] = module_full

  // Add dependency modules
  var depprefix = "%(module_full)_"
  for (dep in info["zig_local_deps"] || []) {
    Emit_module_args.call(dep, depsdir, depprefix)
  }
  for (dep in info["zig_deps"] || []) {
    Emit_module_args.call(dep, dep_depsdir, "%(depprefix)_%(dep["pkg"])_")
  }
}

var main = Fn.new {
  var args = IO.args()[2..-1]
  var depsdir = IO.env("PC_DEPS_DIR") || IO.env("BUILD_DEPS")
  var module_prefix = ""
  for (arg in args) {
    var pkgparse = parse_pkg_str.call(arg)
    Emit_module_args.call(pkgparse, depsdir, module_prefix)
  }
}
main.call()
