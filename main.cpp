/*
 * MIT License
 *
 * Copyright 2026 Michael Goodwin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
#include "version.h"
#include <iostream>
#include <fstream>

namespace options = boost::program_options;
namespace filesystem = boost::filesystem;
namespace json = boost::json;

void print_app_header() {
    std::cout << R"(
===========================================
 JSON
 Version )" << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "." << PROJECT_VERSION_PATCH << R"(
 Author: Michael Goodwin
 Purpose: Perform operations on JSON files
 License: MIT
===========================================
)";
}

void print_help_message() {
    std::cout << R"(
Usage: json [options...] <input>
 -h, --help                  Produce help message
 -p, --print                 Prints the contents of the input file

Example:
    json -p file.json
)";
}

void pretty_print_json(const json::value &value, std::ostream &os, int indent = 0) { // NOLINT(*-no-recursion)
    const std::string indent_str(indent, ' ');
    if (value.is_object()) {
        os << "{\n";
        const auto &obj = value.as_object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            os << indent_str << "  \"" << it->key() << "\": ";
            pretty_print_json(it->value(), os, indent + 2);
            if (std::next(it) != obj.end()) {
                os << ",";
            }
            os << "\n";
        }
        os << indent_str << "}";
    } else if (value.is_array()) {
        os << "[\n";
        const auto &arr = value.as_array();
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            os << indent_str << "  ";
            pretty_print_json(*it, os, indent + 2);
            if (std::next(it) != arr.end()) {
                os << ",";
            }
            os << "\n";
        }
        os << indent_str << "]";
    } else if (value.is_string()) {
        os << value.as_string();
    } else if (value.is_int64()) {
        os << value.as_int64();
    } else if (value.is_uint64()) {
        os << value.as_uint64();
    } else if (value.is_double()) {
        os << value.as_double();
    } else if (value.is_bool()) {
        os << (value.as_bool() ? "true" : "false");
    } else if (value.is_null()) {
        os << "null";
    }
}

int print_value(const json::value &value, const std::string &property) {
    std::vector<std::string> keys;
    std::stringstream ss(property);
    std::string key;
    while (std::getline(ss, key, ':')) {
        keys.push_back(key);
    }

    const json::value *current_value = &value;
    for (const auto &k: keys) {
        if (current_value->is_object() && current_value->as_object().contains(k)) {
            current_value = &current_value->as_object().at(k);
        } else if (current_value->is_array()) {
            try {
                size_t index = std::stoul(k);
                if (index < current_value->as_array().size()) {
                    current_value = &current_value->as_array().at(index);
                } else {
                    std::cout << "Index out of bounds\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument &) {
                std::cout << "Invalid array index\n";
                return EXIT_FAILURE;
            }
        } else {
            std::cout << "Not found\n";
            return EXIT_FAILURE;
        }
    }
    pretty_print_json(*current_value, std::cout);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    try {
        options::options_description desc("Options");
        desc.add_options()
                ("help,h", "produce help message")
                ("version,v", "print version number")
                ("print,p", "prints the contents of the input file")
                ("value,V", options::value<std::string>(), "value to search for")
                ("input", options::value<std::string>(), "input file");

        options::positional_options_description positional_options;
        positional_options.add("input", -1);

        options::variables_map variable_map;
        options::store(options::command_line_parser(argc, argv).options(desc).positional(positional_options).run(),
                       variable_map);
        options::notify(variable_map);

        if (variable_map.count("print")) {
            for (const auto &option: variable_map) {
                if (option.first != "print" && option.first != "input") {
                    std::cerr << "The -p (print) option cannot be used with other options\n";
                    return EXIT_FAILURE;
                }
            }
        }

        if (variable_map.count("help")) {
            print_app_header();
            print_help_message();
            return EXIT_SUCCESS;
        }

        if (variable_map.count("version")) {
            std::cout << "Version: " << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "."
                      << PROJECT_VERSION_PATCH << "\n";
            return EXIT_SUCCESS;
        }

        // Check if input file is provided
        if (!variable_map.count("input")) {
            std::cerr << "Input file is required\n";
            return EXIT_FAILURE;
        }

        // Check if input file exists
        filesystem::path input_file(variable_map["input"].as<std::string>());
        if (!filesystem::exists(input_file)) {
            std::cerr << "Input file does not exist\n";
            return EXIT_FAILURE;
        }

        // Read the contents of the input file
        std::ifstream input_stream(input_file.string());
        json::value json_value = json::parse(input_stream);

        // Print the contents of the input file
        if (variable_map.count("print")) {
            pretty_print_json(json_value, std::cout);
            return EXIT_SUCCESS;
        }

        // Print the value of the property
        if (variable_map.count("value")) {
            std::string value = variable_map["value"].as<std::string>();
            if (print_value(json_value, value) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
        }

    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
