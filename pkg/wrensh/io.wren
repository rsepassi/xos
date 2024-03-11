class Trap {
  construct new(ptr) {
    _ptr = ptr
  }

  cancel() {
    IO.trap_cancel_(_ptr)
  }
}

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
    write_(Fiber.current, s)
    Fiber.yield()
    write_(Fiber.current, "\n")
    Fiber.yield()
  }

  foreign static flush()

  foreign static cwd()
  foreign static chdir(dir)

  static run(argv) {
    run_(Fiber.current, argv)
    return Fiber.yield()
  }

  static run(argv, env) {
    run_(Fiber.current, argv, env)
    return Fiber.yield()
  }

  foreign static exec(argv)
  foreign static exec(argv, env)

  static sleep(n) {
    sleep_(Fiber.current, n)
    Fiber.yield()
  }

  static trap(sig, fn) {
    return Trap.new(trap_(sig, fn))
  }

  foreign static glob(pattern)
  foreign static glob(pattern, dir)

  foreign static exit(c)
  foreign static trap_(sig, fn)
  foreign static trap_cancel_(ptr)

  // private
  foreign static read_(f)
  foreign static write_(f, s)
  foreign static sleep_(f, n)
  foreign static run_(f, args)
  foreign static run_(f, args, env)
}
