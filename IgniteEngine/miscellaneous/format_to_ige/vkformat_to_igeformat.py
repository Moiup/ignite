
f_entry = open("entry.txt", "r")
f_res = open("res.txt", "w")


res = "enum IGEImgFormat\n"
res += "{\n"
f_res.write(res)

for line in f_entry:
    splited_line = line.split(" ")

    if(len(splited_line) > 3):
        continue

    ige_name = splited_line[0]
    ige_name = ige_name.replace("VK_FORMAT_", "")
    ige_name = ige_name.lower() # lower case
    
    res = "    "
    res += ige_name
    res += " = "
    res += splited_line[0] + ",\n"

    f_res.write(res)

f_res.write("};\n")

f_entry.close()
f_res.close()