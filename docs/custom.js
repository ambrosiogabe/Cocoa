window.onload = function () {
    var theme, prefersDarkScheme = window.matchMedia("(prefers-color-scheme: dark)");
    if (prefersDarkScheme.matches)
        theme = document.body.classList.contains("light-mode") ? "light" : "dark";
    else
        theme = document.body.classList.contains("dark-mode") ? "dark" : "light";
    localStorage.setItem("theme", theme);
}

//determines if the user has a set theme
function detectColorScheme(theme) {
    //dark theme preferred, set document with a `data-theme` attribute
    if (theme == "dark") {
        document.documentElement.setAttribute("data-theme", "dark");
    } else {
        document.documentElement.setAttribute("data-theme", "light");
    }
}

function toggleDarkMode() {
    if (document.getElementById('darkModeToggle').checked) {
        detectColorScheme("dark");
        localStorage.setItem("theme", "dark");
    } else {
        detectColorScheme("light");
        localStorage.setItem("theme", "light");
    }
}

SimpleScrollbar.initAll();