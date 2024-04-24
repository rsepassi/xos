#!/usr/bin/env wrensh
var env = IO.env()
env["NEEDTARGET"] = env["HOST"]
env["NEEDDEPS"] = env["BUILD_TOOLS"]
env["NEEDTXT"] = "dyntooldeps.txt"

IO.run(["need"] + IO.args()[2..-1], env)
