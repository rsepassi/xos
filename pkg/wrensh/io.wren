class Process_ {
  construct new(args) {
    _args = args
  }

  call() {
    var pptr = IO.process_(_args, null, null)
    IO.process_wait_(Fiber.current, pptr)
    var ret = Fiber.yield()
    var code = ret[0]
    var stdout = ret[1]
    if (code != 0) return Fiber.abort("Process failed, args[0]=%(_args[0]) code=%(code)")
    return stdout
  }
}

class IO {
  static write(s) {
    write_(Fiber.current, s)
    Fiber.yield()
  }

  static read() {
    read_(Fiber.current)
    return Fiber.yield()
  }

  foreign static flush()
  foreign static arg(i)
  foreign static argc()
  foreign static env(name)
  foreign static exit(c)
  foreign static exec(argv)
  foreign static exec(argv, env)

  static Process { Process_ }

  // private
  foreign static read_(f)
  foreign static write_(f, s)
  foreign static process_(args, env, cwd)
  foreign static process_wait_(f, pid)
}

class Executor_ {
  construct new() {
    _scheduled = []
  }

  async(fn) {
    var root = Fiber.current
    var f = Fiber.new {
      fn.call()
      _runNext()
    }
    _scheduled.add(f)
    f.call()
    return f
  }

  await(fibers) {
    if (fibers is Fiber) fibers = [fibers]
    for (f in fibers) {
      System.print("await suspending")
      while (!f.isDone) Fiber.suspend()
      System.print("await resuming")
    }
  
    var outs = []
    for (f in fibers) {
      outs.add(f.call())
    }
    return outs
  }
}
var X = Executor_.new()
