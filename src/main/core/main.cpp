//
// Created by Khyber on 1/8/2019.
//

#include "src/main/core/main.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "src/lib/fs/fs.h"
#include "src/lib/json/using/Json.h"
#include "src/lib/zip/ZipFile.h"

//#include "llvm/IR/Value.h"

Json parse(const fs::path& path) {
    std::ifstream file(path);
    Json json;
    file >> json;
    return std::move(json);
}

void test1() {
    const auto archive = ZipArchive("/mnt/c/Users/Khyber/Downloads/Maze Starter.sb3");
    for (const auto& entry : archive) {
        std::cout << entry.name() << std::endl;
//        entry.remove();
    }
    
    const Json json = parse("/mnt/c/Users/Khyber/Downloads/project.json");
    std::cout << "json.size(): " << json.size() << std::endl;
    std::cout << "costumes: " << std::setw(4) << json["costumes"] << std::endl;
//    llvm::Value* value = nullptr;
//    std::cout << value << std::endl;
}

int main() {
//    test1();
    
    return EXIT_SUCCESS;
}