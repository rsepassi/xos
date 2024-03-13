foreign class Trap {
  construct new(signal, fn) {}
  foreign cancel()
}

class AsyncValue {
  construct new() {
    _done = false
  }

  err=(err) {
    if (!(err is Null)) {
      _done = true
      _err = err
    }
  }
  val=(val) {
    _done = true
    _val = val
  }

  await() {
    while (!_done) Fiber.yield()
    if (!(_err is Null)) Fiber.abort(_err)
    return _val
  }
}

class X_ {
  construct new() {}

  async(fn) {
    var val = AsyncValue.new()
    var parent = Fiber.current
    var f = Fiber.new {
      val.val = fn.call()
      parent.transfer()
    }
    val.err = f.try()
    return val
  }

  await(vals) {
    var outs = []
    for (v in vals) {
      outs.add(v.await())
    }
    return outs
  }
}
var X = X_.new()

class IO {
  foreign static arg(i)
  foreign static argc()
  foreign static args()
  foreign static env(name)

  static read() {
    read_(Fiber.current)
    return Fiber.yield()
  }

  static write(s) {
    write_(Fiber.current, s)
    Fiber.yield()
  }

  static writeln(s) {
    write_(Fiber.current, "%(s)\n")
    Fiber.yield()
  }

  foreign static flush()

  foreign static cwd()
  foreign static chdir(dir)

  static runs(cmd) {
    run(cmd.split(" "))
    return Fiber.yield()
  }

  static runs(cmd, env) {
    run(cmd.split(" "), env)
    return Fiber.yield()
  }

  static run(argv) {
    run_(Fiber.current, argv)
    return Fiber.yield()
  }

  static run(argv, env) {
    var envl = env
    if (env is Map) {
      envl = []
      for (x in env) {
        envl.add("%(x.key)=%(x.value)")
      }
    }
    run_(Fiber.current, argv, envl)
    return Fiber.yield()
  }

  foreign static exec(argv)
  foreign static exec(argv, env)

  static sleep(n) {
    sleep_(Fiber.current, n)
    Fiber.yield()
  }

  static trap(sig, fn) {
    return Trap.new(sig, fn)
  }

  foreign static glob(pattern)
  foreign static glob(pattern, dir)

  foreign static exit(c)

  // private
  foreign static read_(f)
  foreign static write_(f, s)
  foreign static sleep_(f, n)
  foreign static run_(f, args)
  foreign static run_(f, args, env)
}
