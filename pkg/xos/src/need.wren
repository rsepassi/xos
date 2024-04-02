# WORK IN PROGRESS
# TODO:
# * Need to work out argument serialization/deserialization.
#   Maybe JSON+LMDB. Also would allow for parallelization.
#!/usr/bin/env wrensh

var args = IO.args()
var env = IO.env()

var target = env["NEEDTARGET"] || env["TARGET"]
var opt = env["NEEDOPT"] || env["OPT"]
var deps = env["NEEDDEPS"] || env["BUILD_DEPS"]
var txt = env["NEEDTXT"] || "dyndeps.txt"

var pkg = args[2]
var name = args.count > 3 ? (args[3] == "--" ? pkg : args[3]) : pkg

var dst = "%(deps)/%(name)"

if (!IO.Process(["ls", dst]).test()) {
  System.print("error in need: destination already exists. consider passing a name.")
  System.print("dst=%(dst)")
  IO.exit(1)
}

var depargs = []
var sep = args.indexOf("--")
if (sep != -1) {
  depargs = args[sep..-1]
}

env["TARGET"] = target
env["OPT"] = opt
var pkgid = IO.run(["%(env["XOS_ROOT"])/build", pkg] + depargs, env).trim()
var pkgid_prefix = pkgid[0...2]

// link
IO.run(["mkdir", "-p", deps])
IO.run(["ln", "-s", "%(env["XOS_BUILD_CACHE"])/pkg/%(pkgid_prefix)/%(pkgid)/out", dst])

// log the dynamic dependency
var depsep = "  "
var depargs_quoted = []
for (arg in depargs) {
  depargs_quoted.add("'%(arg)'")
}
var depargs_str = depargs_quoted.join(" ")

var depparts = [
  pkgid,
  name,
  pkg,
  depargs_str,
]
var depline = depparts.join(depsep)

IO.env("V") == "1" ? System.print("need %(depline)") : null
var outf = "%(env["XOS_BUILD_OUT"])/%(txt)"
IO.run(["sh", "-c", "echo \"%(depargs_str)\" >> %(outf)"])
