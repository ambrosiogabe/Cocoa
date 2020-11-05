# CDungeonCrawler
Welcome to Jade Engine (I'll fill this out more later...)

## To Run This
In order to run this, you'll have to run a few commands. First, open up a powershell window in a directory you would like to contain the engine source code. (The engine currently only supports Windows, so this will not work on Linux *yet*).

Next run these commands:
```
git clone --recursive https://github.com/ambrosiogabe/Jade
cd Jade
build.bat
```

These commands will clone the repository into your directory, update the submodules (this step takes awhile because of PhysX), then make sure to contain the latest copies of the submodules (just in case).
After that, simply double click the VisualStudio solution file that was (hopefully) generated, and then once it opens press Ctrl+F5 to run. If all this works properly, you should
be presented with a window saying to create a project or open a project.
