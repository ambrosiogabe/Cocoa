from pathlib import Path

files = list(Path("JadeEngine/cpp").rglob("*.cpp"))

headerFile = open("JadeEngine/cpp/build/build.h", "w")
for file in files:
    if file.__str__()[-8:] != "Main.cpp":
        headerFile.write("#include \"" + file.__str__().replace('\\', '/') + "\"\n")
headerFile.close()

