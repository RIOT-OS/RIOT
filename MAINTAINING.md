# RIOT Maintainer Guidelines

This list presents a series of guidelines for maintainers, including technical
and non-technical. The list is not exhaustive, and represents a baseline on
things that should be ensured for contributions.

Notes:
-   Any of the items in this list might be skipped, if clearly and logically
    articulated reasons are presented.
-   The order of the steps is meant to maximize efficiency and minimize
    overhead, with the philosophy that failing in step n makes steps n+x
    obsolete. However, this efficiency can depend on the quality of the
    submission itself. If the PR (Pull Request) is clearly not in a reviewable
    state (for example, due to poor code cleanliness or overall design), it
    might be more efficient to give the contributor some broader comments for
    improvement before further review.
-   This is a working document and any changes, additions, or other discussions
    are encouraged, via PRs raised against the document. Changes to this
    document should however have at least two ACKs, to ensure these guidelines
    are well thought out, and that they reflect community consensus.

The [list of maintainers] gives information on the current maintainers and the
areas of RIOT they each maintain.


## Technical guidelines

### 1. - Review the fundamentals

Before spending the time on an in-depth code review, it's important to assess
the overall validity of the PR.

1.  Does the reasoning for this PR make sense? \
    Are the requirements and design goals clearly thought out and clearly
    expressed? \
    Is the problem that the PR intends to solve clearly stated?
2.  Is the solution presented in the PR as simple as possible to satisfy the
    requirements, but no simpler?
3.  Is the PR functionally atomic? Does it address a single issue or should
    it be split into multiple individual PRs?
4.  Is the solution well designed on a high level?
5.  Do the concepts used by the PR make sense?
6.  Does the PR break with existing concepts?
7.  Is the structure of the PR itself valid?
8.  Are there clear and adequate instructions on how to test the PR? \
    This may or may not include implemented tests as part of the PR.
9.  Does the code compile and run?
10. Does this PR respect the rights of previous authors, either through
    retaining their commits or by retaining their copyrights in the boilerplate
    headers?
11. Is the PR a duplicate of another PR?


### 2. - Review the design of the code

The following list is not exhaustive and addresses the coding issues we have
regularly seen in the past. In particular, check that the [Best Practices]
are followed. These checks can be aided (but not replaced) by a tool such as
Coccinelle, using the script found in dist/tools/coccinelle.

1.  Check for code duplication
2.  [Check memory usage][Comparing build sizes]
3.  Check all code paths
4.  Check for API compliance
5.  Check for consistent error handling
6.  Check scope of variables and functions
7.  Check for syntactical, semantical or logical errors
8.  Check for any runtime efficiency improvements that can be made in specific
    lines of code - i.e., without changing the overall design of the code
9.  Check that the code design is as simple as possible to solve the problem,
    but no simpler


### 3. - Test the PR

Run tests to verify the correct behavior (see 1.6), both on `native` and on a
few selected boards, or present clearly and logically articulated reasons for
skipping some/all tests.


### 4. - Review the code against the coding conventions

Check that the code follows the [Coding Conventions]. This can be aided (but not
replaced) by Uncrustify, using the uncrustify-riot.cfg file found in the base
directory. Note the difference between personal coding style, which is
subjective and is allowed subject to the other guidelines, and the coding
conventions, which are absolute and must always be followed.


### 5. - Review the documentation

The aim of the documentation is to ensure that the code can be picked up as
easily as possible in the future. Ideally, the documentation is sufficiently
complete that no input from the original developer or maintainer is required.

1.  Check for sufficient high-level (module-level) documentation
2.  Verify function level documentation
3.  Are critical/hard to understand parts in the code documented?
4.  Check grammar and spelling of documentation


## Non-technical guidelines

### Interaction with contributors

-   Be responsive. Even if you are too busy to review the contribution, try to
    add a note fairly soon after the PR is submitted, thanking them for their
    awesome contribution and saying that you will review it in due course. Once
    the contributor has made their changes, ensure you reply to them in a
    reasonable timeframe. Acknowledge their replies to concerns if you're happy
    with their argument.
-   Be helpful. Give precise and correct advice when possible and when it will
    help the contributor. This can include code snippets, links to
    code/issues/wiki entries/web pages, or anything else. Educating contributors
    means we are investing in our community.
-   Be friendly. Be objective in your review, and of course, follow the
    [Code of Conduct].


### Organisation of reviewing between maintainers

This section is a placeholder for further discussion around making the review
process more efficient and effective when multiple maintainers are involved.
This could include:

-   Usage of labels
-   Division of review responsibilities, e.g. with ACKs for different areas of
    review
-   Usage of GitHub functionality, for example "Reviewers" and "Assignees" lists

#### Github etiquette

-   If there are multiple maintainers reviewing a PR, always give the other
    maintainers reasonable time to ACK before dismissing their review.

[list of maintainers]: https://github.com/RIOT-OS/RIOT/wiki/Maintainers
[Best Practices]: https://github.com/RIOT-OS/RIOT/wiki/Best-Practice-for-RIOT-Programming
[Comparing build sizes]: https://github.com/RIOT-OS/RIOT/wiki/Comparing-build-sizes
[Coding Conventions]: https://github.com/RIOT-OS/RIOT/wiki/Coding-conventions
[Code of Conduct]: https://github.com/RIOT-OS/RIOT/blob/master/CODE_OF_CONDUCT.md
