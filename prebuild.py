from pathlib import Path

files = list(Path("src/cpp").rglob("*.cpp"))

headerFile = open("src/cpp/build/build.h", "w")
for file in files:
    if file.__str__()[-11:] != "winMain.cpp":
        headerFile.write("#include \"" + file.__str__().replace('\\', '/') + "\"\n")
headerFile.close()

