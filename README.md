# C++ Package Manager

This project simulates how a real package manager works. It consists of three applications.

## Client Package Manager

Allows users to install packages with full dependency checking and data validation. It can also remove packages if the check shows that no other packages use them.

Currently, three types of packages are supported. Checks that require a full traversal of the dependency graph run in multithreaded mode.

## Server

An application that listens for TCP connections and handles requests. A request can come from:

- A regular client (for example, to update the local package list)
- An administrator (to modify the repositories stored on the server)

All changes are validated by the package manager in the same way as in the client application.

## Administrator Utility

Allows updating the state of repositories on the server.

## Additional Features

The project includes a custom template hash table with support for user-defined memory allocators.

## Main Features

The main functionality includes client package manager operations such as:

- Adding a package
- Removing a package
- Removing unused packages in multithreaded mode

When you start the client application, you will see the following menu:

```text
=========================MENU===========================

 1. Add package------------------------------------------
 2. Remove package -------------------------------------
 3. Find package-----------------------------------------
 4. Remove unused----------------------------------------
 6. Print size-------------------------------------------
 7. Global update----------------------------------------
 9. EXIT-------------------------------------------------

 ========================================================


## How It Works

Besides the client package manager, there is also a server. You can download updated JSON repositories with new package lists from this server. The administrator utility allows changing the state of these repositories on the server.

This project tries to model how the `apt` package manager works. The simulation does not include the actual installation of software from these packages. It is a simple model.

## Testing

For convenience, there is a separate folder called `testing_field`. You can use it to try running these programs with pre-made test repositories.
