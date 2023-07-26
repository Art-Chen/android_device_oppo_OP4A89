import os

def process_te(filename: str):
    print("process: ", filename)
    typeStr = filename.removesuffix(".te")
    f = open(filename, "w+")
    typeContentStr = "# HwBinder IPC from client to server, and callbacks\n" + \
            "binder_call({0}_client, {0}_server)\n" + \
            "binder_call({0}_server, {0}_client)\n\n" + \
            "hal_attribute_hwservice({0}, {0}_hwservice)\n"
    defaultContent = "type {0}_default, domain;\n" + \
            "hal_server_domain({0}_default, {0})\n\n" + \
            "type {0}_default_exec, exec_type, vendor_file_type, file_type;\n" + \
            "init_daemon_domain({0}_default)\n"
    f.write(typeContentStr.format(typeStr))
    f.close()

    f_default = open(typeStr + "_default.te", "w+")
    f_default.write(defaultContent.format(typeStr))
    f_default.close()



def do_process():
    files = os.listdir()
    for f in files:
        print("current file: ", f)
        if f == "process.py" or f.endswith("_default.te"):
            print("skipped! ", f)
            continue
        process_te(f)



if __name__ == "__main__":
    do_process()