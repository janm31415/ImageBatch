#include "CmdList.h"
#include <iostream>

CmdList::CmdList(const std::string& application_name, const std::string& general_help_text, 
  const std::string& optional_arguments_help_text) : app_name(application_name), opt_arg_help(optional_arguments_help_text)
  {
  RegCmd("?", general_help_text,
    [this](std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)
    {
    PrintHelp(std::cout);
    });
  }

CmdList::~CmdList()
  {
  }

void CmdList::RegCmd(const std::string& name,
  const std::string& helptxt,
  const std::function<void(std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)>& body)
  {
  command cmd;
  cmd.name = name;
  cmd.helptxt = helptxt;
  cmd.body = body;
  commands.push_back(cmd);
  }

void CmdList::PrintHelp(std::ostream& out)
  {
  out << app_name << std::endl;
  out << "  Usage:" << std::endl;
  out << "    \\> " << app_name << " -command arg0 arg1 ... argn <opt_arg0 ... >" << std::endl;
  if (!opt_arg_help.empty())
    {
    out << "  Optional arguments:" << std::endl;
    out << opt_arg_help << std::endl;
    }
  out << "  Commands:" << std::endl;
  for (auto& cmd : commands)
    {
    out << cmd.helptxt << std::endl;
    }
  out << std::endl;
  }

bool CmdList::RunCommands(std::vector<std::string>::iterator& arg_it, const std::vector<std::string>::iterator& end_it)
  {

  std::string arg_cmd = *arg_it;
  if (arg_cmd[0] != '-')
    {
    std::cerr << arg_cmd << std::endl;
    std::cerr << "All commands must begin with '-'" << std::endl;
    return false;
    }
  arg_cmd = arg_cmd.substr(1);
  ++arg_it;
  for (auto& cmd : commands)
    {
    if (arg_cmd == cmd.name)
      {
      cmd.body(arg_it, end_it);
      return true;
      }
    }
  std::cerr << "Command -" << arg_cmd << " is not recognized" << std::endl;
  return false;
  }