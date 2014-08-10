#include <iostream>
#include <unistd.h>

#include "arguments.h"

std::string Arguments::load_model;

void
Arguments::init (int argc, char** argv)
{
  for (int idx = 1; idx < argc; ++idx)
  {
    std::string arg(argv[idx]);
    if (arg == "-m" || arg == "--model")
    {
      if (Arguments::has_argument(argc, argv, idx))
      {
        Arguments::load_model = argv[idx + 1];
        idx += 1;

        /* Make it absolute. */
        if (Arguments::load_model[0] != '/')
        {
          char buffer[512];
          char* cwd = ::getcwd(buffer, 512);
          if (cwd != 0)
          {
            Arguments::load_model = std::string(buffer)
                + "/" + Arguments::load_model;
          }
        }
      }
    }
    else if (arg == "-h" || arg == "--help")
    {
      Arguments::show_help();
    }
    else
    {
      std::cout << argv[idx] << ": Argument not recognized" << std::endl;
    }
  }
}

/* ---------------------------------------------------------------- */

void
Arguments::show_help (void)
{
  std::cout << "Usage: gtkevemon [ options ]" << std::endl
      << "Options:" << std::endl
      << "  -h, --help                Display this helpful text" << std::endl
      << "  -m MODEL, --model MODEL   Load model after startup" << std::endl;
}

/* ---------------------------------------------------------------- */

bool
Arguments::has_argument (int argc, char** argv, int idx)
{
  if (argc <= idx + 1 || argv[idx + 1][0] == '\0' || argv[idx + 1][0] == '-')
  {
    std::cout << argv[idx] << ": Expecting argument for option" << std::endl;
    return false;
  }

  return true;
}
