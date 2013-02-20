git-svn bootstrap repository for freebsd-base
=============================================

What is this branch?
--------------------

This branch serves as a bootstrap for getting your own copy of a
git-svn enabled mirror repository of FreeBSD "src" a.k.a. "base".

It will include all available branches.  You can refer to any commit
on any branch using Git and see what revision it corresponds to.

How much disk space do I need?
------------------------------

You'll need more than 2GB of disk space to have a git-svn clone of the
FreeBSD base repository.

How long does it take?
----------------------

It highly depends on the bandwidth, but in my case the whole process
finished within an hour.

How to use
----------

Take the following steps to get a git-svn working directory:

1. Clone this repository.  The git URL is preferred to https if you
   want to save time as much as possible.

        $ git clone git://github.com/knu/freebsd.git

   If the above command causes access error, use the https URL
   instead.

        $ git clone https://github.com/knu/freebsd.git

2. Go into the working directory and make sure you are on the
   `git-svn` branch.

        $ cd freebsd
        $ git status
        # On branch git-svn
        nothing to commit, working directory clean

3. Execute `./run init`.  This sets up the repository for git-svn,
   extracting a snapshot of git-svn files and refs.

  * If you have a local mirror of the subversion repository:

        $ ./run init --repository=file:///path/to/repository

  * If you are a non-committer:

        $ ./run init --repository=svn://svn.freebsd.org/base

  * If you are a committer:

        $ ./run init

4. Execute `./run fetch`.  This simply runs `git-svn fetch` to obtain
   the new commits since the snapshot was generated.

        $ ./run fetch

5. Execute `./run clean`.  This completely erases the trace of this
   bootstrap branch and changes the `origin` URL from
   [this fork](https://github.com/knu/freebsd) to
   [the official mirror](https://github.com/freebsd/freebsd).

        $ ./run clean

6. Start hacking!  Note that the `master` branch is branched off from
   [`origin/master`](https://github.com/freebsd/freebsd/tree/master)
   and is not set up to track any subversion branch with git-svn.  To
   use commands like `git svn rebase` and `git svn dcommit`, create a
   branch from one of the remote branches that start with `svn/` and
   work in there.

        $ git checkout -b trunk svn/trunk
        (..hack..)
        $ git svn rebase
        $ git svn dcommit

Author
------

The script and this document is:

Copyright (c) 2013 Akinori MUSHA.

Licensed under the 2-clause BSD license.  See `LICENSE.txt` for
details.

Visit [GitHub Repository](https://github.com/knu/freebsd) for the
latest information.
