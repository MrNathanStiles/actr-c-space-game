To add headers to your project
git subtree add --prefix header https://github.com/MrNathanStiles/actr-c-header.git main --squash

To update latest headers run the following command
git subtree pull --prefix header https://github.com/MrNathanStiles/actr-c-header.git main --squash -m "update header files"

To run the auto build and publish on commit:
You may need to re-enable actions in in your repository
Generate an API key from the program manager
Visit https://www.d1ag0n.com/ng/program/manager to access the program manager and generate an API key.
Add the API key to github by clicking settings, under Secrets and Variables on the left menu click actions.
Click the "New Repository Secret" button
Name the secret API_UPLOAD_KEY and paste the API key into the value area.
When you make a commit the changes will be built and uploaded by the github runner.

Visit the discord server if you need help or to show off what you've created https://discord.gg/vDE8EnttaZ
