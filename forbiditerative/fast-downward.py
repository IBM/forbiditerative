#! /usr/bin/env python3

if __name__ == "__main__":
    from driver.main import main
    from forbiditerative.plan import set_default_build_path
    set_default_build_path()
    main()
