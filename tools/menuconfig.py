import subprocess
import os


def main():
    subprocess.run(["scons", "--menuconfig"])
    # os.system("arm-none-eabi-objdump -d ./rtthread-stm32f4xx.elf >./rtthread.s")
    # os.system("/Users/huanghuisheng/.env/tools/scripts/pkgs ~/.env/env.sh")
    # os.system("export ALL_PROXY=socks5://127.0.0.1:1081")
    subprocess.run(["echo", "update env......"])
    # os.system("/Users/huanghuisheng/.env/tools/scripts/pkgs --upgrade")
    subprocess.run(["echo", "update packs......"])
    os.system("~/.env/tools/scripts/pkgs --update")
    # os.system("unset ALL_PROXY")
    subprocess.run(["scons", "--target=vsc", "-s"])
    # subprocess.run(["scons", "--target=mdk5", "-s"])
    subprocess.run(["echo", "Building......."])
    subprocess.run(["python3", "./tools/rebuild.py"])


if __name__ == "__main__":
    main()
