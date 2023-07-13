package main

import (
	"syscall"
	"unsafe"
  "io/ioutil"
  "fmt"
  //"plugin"
)

func CreateFD() uintptr {
	var fdid uintptr = 319
	var name string = ""
  fdexe := 0x0001
  fd, _, err := syscall.Syscall(fdid, uintptr(unsafe.Pointer(&name)), uintptr(fdexe), 0)
  if err != 0 {
    println(err.Error())
  }
  return fd
}

func cptofd(fd uintptr, path string) {
  buff, err := ioutil.ReadFile(path)
  if err != nil {
    println(err.Error())
  }
  _,err = syscall.Write(int(fd),buff)
  if err != nil {
    println(err.Error())
  }
}

func call(fd uintptr) {
  fdPath := fmt.Sprintf("/proc/self/fd/%d",fd)
  err := syscall.Exec(fdPath, []string{"xxx"}, nil)
  if err != nil {
    println(err.Error())
  }
}

func main() {
  x := CreateFD()
  cptofd(x,"libshare")
  call(x)
}
