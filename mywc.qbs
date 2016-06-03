import qbs 1.0

Application {
    name: "mywc"
    consoleApplication: true

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++11"

    files: [
        "*.h",
        "*.cpp"
    ]
}
