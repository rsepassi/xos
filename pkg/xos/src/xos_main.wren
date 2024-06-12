#!/usr/bin/env wrensh

class Build {
  construct new(args) {
    _args = args
  }

  run() {
    var argv = ["%(IO.env("XOS_ROOT"))/tools/build"] + _args
    IO.Process(argv).stdout(1).stderr(2).run()
  }
}

var CMDS = {
  "build": Build,
}

var main = Fn.new {
  var args = IO.args()
  var cmd = args[2]
  if (!CMDS.containsKey(cmd)) {
    System.print("unrecognized command %(cmd)\navailable: %(CMDS.keys.toList)")
    IO.exit(1)
  }
  var cmd_args = args[3..-1]
  CMDS[cmd].new(cmd_args).run()
}

main.call()
