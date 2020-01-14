import subprocess
import os


def main():
    subprocess.run(["echo", "\n\r=============building============\n\r"])
    subprocess.run(["scons", "-j6"])
    # subprocess.run(["echo", "\n\r=========汇编文件生成中==========\n\r"])
    os.system("/home/hehe/xPacks/riscv-none-embed-gcc/8.3.0-1.1/bin/riscv-none-embed-objdump -d ./rtthread.elf >./rtthread.asm")
    # subprocess.run(["echo", "\n\r=========解析ELF文件中==========\n\r"])
    os.system("/home/hehe/xPacks/riscv-none-embed-gcc/8.3.0-1.1/bin/riscv-none-embed-readelf -a ./rtthread.elf >./rtthreadelf.asm")
    # subprocess.run(["echo", "\n\r==============成功===============\n\r"])
    # subprocess.run(["make", "all"])
    # subprocess.run(["make", "change"])
    # subprocess.run(["arm-none-eabi-objdump", "-d",
    #                 "./OBJ/Output.elf", ">./OBJ/Output.s"])
    # subprocess.run(["make", "delete"])


if __name__ == "__main__":
    main()
