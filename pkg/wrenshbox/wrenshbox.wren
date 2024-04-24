
var echo = Fn.new {
  var nl = true

  var argstart = 1
  if (IO.arg(argstart) == "-n") {
    argstart = 2
    nl = false
  }

  for (i in argstart...IO.argc()) {
    if (i > argstart) IO.write(" ")
    IO.write(IO.arg(i))
  }

  if (nl) IO.write("\n")
}

var basename = Fn.new {
  IO.writeln(IO.arg(1).split("/")[-1])
}

var dirname = Fn.new {
  IO.writeln(IO.arg(1).split("/")[0...-1].join("/"))
}

var vlog = Fn.new {
  var v = IO.env("V")
  if (v == null) return
  v = Num.fromString(v)
  var args = IO.args()
  if (Num.fromString(args[1]) >= v) {
    for (arg in args[2...-1]) {
      IO.writeln(arg)
    }
  }
}

var cmds = {
  "basename": basename,
  "dirname": dirname,
  "echo": echo,
  "vlog": vlog,
}

var main = Fn.new {
  var cmd = IO.arg(0).split("/")[-1]
  if (cmd == "wrenshbox") {
    IO.writeln("wrenshbox commands: %(cmds.keys.toList)")
    return
  }

  cmds[cmd].call()
}
main.call()
