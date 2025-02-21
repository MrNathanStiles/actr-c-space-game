To add headers to your project
git subtree add --prefix header https://github.com/MrNathanStiles/actr-c-header.git main --squash

To update latest headers run the following command
git subtree pull --prefix header https://github.com/MrNathanStiles/actr-c-header.git main --squash -m "update header files"
