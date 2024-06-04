#!/usr/bin/env python3

# Copyright (C) 2024 Nam Nguyen
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import requests
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import shutil


def get_makefiles():
    # Get the makefiles and read the PKG_URL and PKG_VERSION
    file_contents = {}
    # Get the current working directory
    relative_path = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'pkg')
    current_dir = os.path.abspath(relative_path)
    # Iterate through each item in the current directory
    for item in os.listdir(current_dir):
        # Construct the full path of the item
        item_path = os.path.join(current_dir, item)
        # Check if the item is a directory
        if os.path.isdir(item_path):
            # Construct the path of the Makefile within the directory
            makefile_path = os.path.join(item_path, 'Makefile')
            # Check if Makefile exists in the directory
            if os.path.isfile(makefile_path):
                # Open the Makefile and read its content
                with open(makefile_path, 'r') as file:
                    content = file.readlines()
                # Iterate through each line to find PKG_VERSION and PKG_URL
                pkg_version = None
                pkg_url = None
                for line in content:
                    # Check if line contains PKG_URL
                    if line.strip().startswith("PKG_URL"):
                        # Extract the value of PKG_URL
                        pkg_url = line.strip().split("=")[1]
                        # Process space and comments
                        if pkg_url[0] == " ":
                            pkg_url = pkg_url[1:]
                        pkg_url = pkg_url.split(" ")[0]
                    # Check if the line contains PKG_VERSION
                    if line.strip().startswith("PKG_VERSION"):
                        # Extract the value of PKG_VERSION
                        pkg_version = line.strip().split("=")[1]
                        # Process space and comments
                        if pkg_version[0] == " ":
                            pkg_version = pkg_version[1:]
                        pkg_version = pkg_version.split(" ")[0]
                # If PKG_VERSION was found, store it in the dictionary
                if pkg_version:
                    file_contents[item_path] = [pkg_url, pkg_version]
    return file_contents


def process_url(repo_url):
    # Remove the '.git' suffix from the repository URL if it is present
    if repo_url.endswith(".git"):
        repo_url = repo_url[:-4]
    return repo_url


def get_hash_from_branch(repo_url, branch, headers):
    # Get the latest commit hash from the specified branch of the GitHub repository
    commit_url = f"{repo_url}/commits/{branch}"
    commit_response = requests.get(commit_url, headers=headers)
    commit_response.raise_for_status()  # Raise an exception for HTTP errors
    commit_data = commit_response.json()
    return commit_data["sha"]


def get_latest_hash(repo_url, headers):
    # Get the latest commit hash from the GitHub repository
    repo_url = process_url(repo_url)
    # Try to get the commit hash from the main branch
    try:
        return get_hash_from_branch(repo_url, "main", headers)
    except requests.HTTPError:
        # If the main branch is not found, try to get the commit hash from the master branch
        try:
            return get_hash_from_branch(repo_url, "master", headers)
        except requests.HTTPError:
            try:
                return get_hash_from_branch(repo_url, "dev", headers)
            except Exception:
                print("Failed to fetch the latest commit hash")
                return None


def get_release_or_commit_hash(repo_url, headers):
    # Get the latest release or commit hash from the GitHub repository
    repo_url = process_url(repo_url)
    # Get the latest release data from the GitHub API
    release_url = f"{repo_url}/releases/latest"
    release_response = requests.get(release_url, headers=headers)
    # Try to get the latest release commit hash
    if release_response.status_code == 200:
        release_data = release_response.json()
        tag_name = release_data.get("tag_name")
        if tag_name:
            commit_url = f"{repo_url}/git/refs/tags/{tag_name}"
            commit_response = requests.get(commit_url, headers=headers)
            commit_response.raise_for_status()  # Raise an exception for HTTP errors
            commit_data = commit_response.json()
            return commit_data["object"]["sha"]
    # Fall back to fetching the latest commit hash from the main/master branches
    return get_latest_hash(repo_url, headers)


def check_repository(key, item, api_url, base_url, headers, lock, outdated_packages, failures):
    # Check a repository for outdated packages
    if not item[0].startswith("https://github.com"):
        with lock:
            filename = os.path.basename(key)
            failures.append(filename)
        return
    api_repo_url = api_url + item[0][len(base_url):]
    if api_repo_url.endswith("/"):
        api_repo_url = api_repo_url[:-1]
    latest_release = get_release_or_commit_hash(api_repo_url, headers)
    if item[1] != latest_release:
        # Acquire lock and update outdated count
        with lock:
            filename = os.path.basename(key)
            outdated_packages.append(filename)


def print_columns(items, num_columns=3):
    # Get the terminal width
    terminal_width = shutil.get_terminal_size().columns
    # Calculate the width of each column
    column_width = terminal_width // num_columns
    # Calculate the number of rows needed (distributed evenly)
    num_rows = (len(items) + num_columns - 1) // num_columns
    # Create a list of rows for each column
    columns = [[] for _ in range(num_columns)]
    for i, item in enumerate(items):
        columns[i % num_columns].append(item)
    # Print items in columns
    for row in range(num_rows):
        row_items = []
        for col in range(num_columns):
            if row < len(columns[col]):
                row_items.append(columns[col][row].ljust(column_width))
            else:
                row_items.append("".ljust(column_width))
        print("".join(row_items))


def main_func():
    api_url = "https://api.github.com/repos/"
    base_url = "https://github.com/"
    # Define GitHub API token here
    github_token = input("Enter GitHub API token: ")
    # Create a headers dictionary with the token
    headers = {
        "Authorization": f"token {github_token}"
    }
    outdated_packages = []
    failures = []
    # Lock for synchronizing access to the outdated_count variable
    lock = threading.Lock()
    versions = get_makefiles()
    print("Checking for outdated packages...")
    with ThreadPoolExecutor() as executor:
        futures = []
        # Submit tasks to the executor
        for key, item in versions.items():
            future = executor.submit(check_repository, key, item,
                                     api_url, base_url, headers, lock,
                                     outdated_packages, failures)
            futures.append(future)
        # Handle each future as it completes
        for future in as_completed(futures):
            try:
                # Get the result (if any) from the future
                future.result()
            except Exception as e:
                print(f"Cannot get result from future: {e}")
    # Print total number of outdated packages
    print(f"\nTotal number of outdated packages: {len(outdated_packages)}")
    print(f"Total number of failures: {len(failures)}")
    print("\nOutdated packages: ")
    print_columns(outdated_packages)
    print("\nPackages that couldn't be checked: ")
    for package in failures:
        print(package)


if __name__ == "__main__":
    main_func()
