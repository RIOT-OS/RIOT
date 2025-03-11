# RIOT Maintainer Guidelines

This list presents a series of technical and non-technical guidelines for maintainers.
The list is not exhaustive, it represents a baseline on
things that should be ensured for contributions.

Notes:
-   Any of the items in this list can be skipped, if clearly and logically
    articulated reasons are presented.
-   The order of the steps is meant to maximize efficiency and minimize
    overhead, with the philosophy that failing in step n makes steps n+x
    obsolete. However, this efficiency can depend on the quality of the
    submission itself. If the PR (Pull Request) is clearly not in a reviewable
    state (for example, due to poor code cleanliness or overall design), it
    might be more efficient to give the contributor some broader comments for
    improvement before further review.
-   This is a working document: any changes, additions, or other discussions
    are encouraged, via PRs raised against the document. Changes to this
    document should have at least two ACKs, to ensure these guidelines
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
3.  Is the PR a manageable size? It should be confined to a single explainable
    change, and be runnable on its own.
4.  Is the solution well designed on a high level?
5.  Do the concepts used by the PR make sense?
6.  Does the PR break with existing concepts?
7.  Is there a clean commit history in the pulled branch? The commit history
    should group the code differences cleanly.
8.  Are there clear and adequate instructions on how to test the PR? \
    This may or may not include implemented tests as part of the PR.
9.  Does the code compile and run?
10. Does this PR respect the rights of previous authors, either through
    retaining their commits or by retaining their copyrights in the boilerplate
    headers?
11. Is the PR a duplicate of another PR?


### 2. - Review the design of the code

The following list is not exhaustive, it addresses the coding issues we have
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
directory. Note the difference between personal coding style, which is allowed
subject to the other guidelines, and the coding conventions, which are absolute
and must always be followed.


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
    valuable contribution and saying that you will review it in due course. Once
    the contributor has made their changes, ensure you reply to them in a
    reasonable timeframe. Acknowledge their replies to concerns if you're happy
    with their argument.
-   Be helpful. Give precise and correct advice when possible and when it will
    help the contributor. This can include code snippets, links to
    code/issues/wiki entries/web pages, or anything else. Educating contributors
    means we are investing in our community.
-   Be friendly. Respect the original author, bearing in mind that their coding
    style or their design may be just as valid as the way you would have done
    it and of course, always follow the [Code of Conduct].
-   If a contributor has opened a PR, the reviewer should attempt to
    help the author of the contribution to get it to its best shape and
    according to the RIOT Standard™. If there is disagreement, it’s important
    to understand the reasons behind and always give technical arguments,
    pros and cons or snippets.


### Organisation of reviewing between maintainers

#### Partial review

You can review a PR partially. This would involve reviewing all points in one or
more sections outlined in the [technical guidelines](#technical-guidelines).
In that case, please do not "approve" the PR to prevent accidental merges.
Rather, give your verbal ACK and describe what you reviewed. In addition, if you
processed or reasonably stepped over a whole section, mark the PR with the
according label from the "Reviewed:" category. If you set a label by stepping
over a section, please articulate your reasoning for this clearly, as noted in
the [introduction](#introduction). This will help other maintainers to
better understand your line of thought. If you disagree with the assessment of
a previous review, you may remove a certain "Reviewed:" label. Please state your
reasoning in this case as well.

When all "Reviewed:" labels are set you may give your approval for the PR.

As for everything in this document this is a "CAN", not a "MUST": It might help
other maintainers to track your work, but if the overhead isn't justified, a
simple approving ACK might suffice.

#### Github etiquette

It is good etiquette to describe what you reviewed, even if you gave the PR a
full review and gave your approval. This way the contributor and other
maintainers are able to follow your thought process.

Maintainers should only assign themselves to PRs and shouldn't assign other
maintainers. You can however request reviews from other maintainers or
contributors, either by mentioning them in a comment or selecting them in
GitHub's review sidebar.

If there are multiple maintainers reviewing a PR, always give the other
maintainers reasonable time to ACK before dismissing their review.

### Release, Feature Freeze, and Backports

Before the official release of a new RIOT version, two feature freeze periods
are announced on the
[RIOT maintainer forum](https://forum.riot-os.org/c/maintainer/6):
The soft feature freeze and the hard feature freeze. During the soft feature
freeze only PRs with minor impact should be merged into master. The hard feature
freeze begins when the release manager creates a new release branch. Therefore,
the restriction on merging PRs into the master branch are lifted at that point.

Once the release branch is created, no backports of new features will be
accepted. Instead, backports should consist only of bug fixes or of reverting
features that were added during the last development period (and not part of any
release), but didn't reach the required maturity or API stability yet. For
bigger changes (which explicitly includes any revert), the PR has to be
announced to the maintainer mailing list and should be merged no sooner than
48h after the announcement and needs at least two ACKs.

In case of security relevant backports (both bug fixes and reverts), the
announcement can be skipped and the fix merged once at least two ACKs are
there.

[list of maintainers]: https://riot-os.org/maintainers.html
[Best Practices]: https://doc.riot-os.org/dev-best-practices.html
[Comparing build sizes]: https://doc.riot-os.org/advanced-build-system-tricks.html#comparing-build-sizes
[Coding Conventions]: CODING_CONVENTIONS.md
[Code of Conduct]: https://github.com/RIOT-OS/RIOT/blob/master/CODE_OF_CONDUCT.md
