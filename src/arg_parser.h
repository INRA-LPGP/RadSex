#pragma once
#include <iostream>
#include <stdio.h>
#include "CLI11/CLI11.hpp"
#include "parameters.h"
#include "utils.h"


inline std::string failure_message(const CLI::App* parser, const CLI::Error& error) {

    std::string message = "";

    if (error.what() == std::string("A subcommand is required")) {
        message = "\nSubcommand error: missing or invalid subcommand\n\n" + parser->help();
    } else if (error.get_exit_code() == 106) {  // 106 corresponds to wrong argument type
        message = "\nArgument error: " + std::string(error.what()) + "\n\n" + parser->help();
    } else {
        message = "\nError: " + std::string(error.what()) + "\n\n" + parser->help();
    }

    return message;
}


// Formatter for CLI
class CustomFormatter : public CLI::Formatter {

    public:

        uint column_widths[3] {0, 0, 0};  // Will be used to store the maximum width of each column : flags, type, description
        uint border_width = 4;  // Space between two columns

        // Formatter for an Option line, overrides the same function from CLI::Formatter
        virtual std::string make_option(const CLI::Option* opt, bool is_positional) const {

            std::string option = "", name = "", type = "", description = "", default_value = "", required = "REQUIRED", short_name = "";
            std::vector<std::string> options;

            // Generate option name, if positional -> just the name, if not positional -> <short_flag, long_flag>
            if (is_positional) {
                name = opt->get_name();
            } else {
                options = split(opt->get_name(false, true), ",");
                options.size() == 1 ? name = options[0] : name = options[1] + ", " + options[0];
            }
            type = opt->get_type_name();
            description = opt->get_description();
            default_value = opt->get_default_str();

            // Generate the help string for this option, adding the right number of spaces after each column based on column_widths
            option = name + std::string(border_width + column_widths[0] - name.size(), ' ');
            option += type + std::string(border_width + column_widths[1] - type.size(), ' ');
            option += description + std::string(border_width + column_widths[2] - description.size(), ' ');
            if (opt->get_required()) default_value = required;
            if (default_value != "") option += "[" + default_value + "]";
            option += "\n";

            return option;
        }

        virtual std::string make_description(const CLI::App *app) const {

            return "";
        }

        void set_column_widths(CLI::App& parser) {
            std::string tmp = "";
            for (auto opt: parser.get_subcommands()[0]->get_options()) {
                tmp = opt->get_name();
                if (tmp.size() > this->column_widths[0]) this->column_widths[0] = static_cast<uint>(tmp.size());
                tmp = opt->get_type_name();
                if (tmp.size() > this->column_widths[1]) this->column_widths[1] = static_cast<uint>(tmp.size());
                tmp = opt->get_description();
                if (tmp.size() > this->column_widths[2]) this->column_widths[2] = static_cast<uint>(tmp.size());
            }
            this->column_widths[0] += 2;
        }

};


//// Formatter for CLI
//class CustomFormatter : public CLI::Formatter {

//    public:

//        uint column_widths[3] {0, 0, 0};  // Will be used to store the maximum width of each column : flags, type, description
//        uint border_width = 4;  // Space between two columns

//        // Formatter for an Option line, overrides the same function from CLI::Formatter
//        virtual std::string make_option(const CLI::Option* opt, bool is_positional) const {

//            std::string option = "", name = "", type = "", description = "", default_value = "", required = "REQUIRED";

//            // Generate option name, if positional -> just the name, if not positional -> <short_flag, long_flag>
//            is_positional ? name = opt->get_name()[0] : name = "-" + opt->get_snames()[0] + ", --" + opt->get_lnames()[0];
//            type = opt->get_type_name();
//            description = opt->get_description();
//            default_value = opt->get_defaultval();

//            // Generate the help string for this option, adding the right number of spaces after each column based on column_widths
//            option = name + std::string(this->border_width + this->column_widths[0] - name.size(), ' ');
//            option += type + std::string(this->border_width + this->column_widths[1] - type.size(), ' ');
//            option += description + std::string(this->border_width + this->column_widths[2] - description.size(), ' ');
//            if (opt->get_required()) default_value = required;
//            if (default_value != "") option += "[" + default_value + "]";
//            option += "\n";

//            return option;
//        }
//};
