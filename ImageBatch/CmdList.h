#pragma once

#include <string>
#include <functional>
#include <vector>

class CmdList
  {
  public:
    CmdList(const std::string& application_name, const std::string& general_help_text, const std::string& optional_arguments_help_text);
    ~CmdList();

    void RegCmd(const std::string& name,
      const std::string& helptxt,
      const std::function<void(std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)>& body);

    void PrintHelp(std::ostream& out);

    bool RunCommands(std::vector<std::string>::iterator& arg_it, const std::vector<std::string>::iterator& end_it);

  private:

    struct command
      {
      std::string name;
      std::string helptxt;
      std::function<void(std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)> body;
      };

    std::vector<command> commands;
    std::string app_name, opt_arg_help;
  };