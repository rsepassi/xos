IO.writeln("hi")
IO.write("hello ")
IO.write("world\n")
IO.writeln([IO.args(), IO.argc(), IO.arg(0)])
IO.writeln(IO.env())
IO.writeln(IO.env("HOME"))
IO.flush()
IO.writeln(IO.cwd())
IO.writeln(IO.run(["ls", "-l"]))
IO.writeln(IO.glob("r*"))
IO.writeln(IO.glob("l*/build.sh", "pkg"))

var timer = IO.Timer()
var t1 = timer.read()
IO.sleep(500)
var t2 = timer.read()

var kv = KV.new("/tmp/mydb")
kv.set("a", "b")
kv.set("a.b", "c")
IO.writeln(kv.get("a"))
IO.writeln(kv.getp("a"))

var Point = Data.Record("Point", ["x", "y"])
var p = Point.new(3, 4)
IO.writeln(p)

var val = [1, 2, [3, 4]]
var encoded = JSON.encode(val)
IO.writeln(encoded)
IO.writeln(JSON.decode(encoded))
var t3 = timer.read()

IO.writeln([t1, t2, t3])

var sleep = Fn.new {
  IO.sleep(500)
}

var asyncs = []
for (i in 0...3) {
  System.print(i)
  asyncs.add(X.async(sleep))
}
X.await(asyncs)
IO.writeln(timer.read())

IO.chdir("build")
IO.writeln(IO.cwd())
IO.writeln(IO.run("ls"))

IO.writeln("code=%(IO.runc("which foo"))")
