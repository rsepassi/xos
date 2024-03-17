
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

var cmds = {
  "basename": basename,
  "dirname": dirname,
  "echo": echo,
}

var main = Fn.new {
  cmds[IO.arg(0).split("/")[-1]].call()
}
main.call()
