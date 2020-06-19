from pathlib import Path

files = list(Path("jade/cpp").rglob("*.cpp"))

headerFile = open("jade/cpp/build/build.h", "w")
for file in files:
    if file.__str__()[-8:] != "Main.cpp":
        headerFile.write("#include \"" + file.__str__().replace('\\', '/') + "\"\n")
headerFile.close()

