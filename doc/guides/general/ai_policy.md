---
title: AI Policy
description: RIOT's policy on the use of AI tools for contributions to the project.
---

## Policy

RIOT's policy is that contributors can use whatever tools they would like to
craft their contributions, but there must be a **human in the loop**.
**Contributors must read and review all LLM-generated code and text before they
ask other project members to review it.** The contributor is always the author
and is fully accountable for their contributions. Contributors should be
sufficiently confident that the contribution is high enough quality that asking
for a review is a good use of scarce maintainer time, and they should be **able
to answer questions about their work by themselves** during review.
Contributions that use AI should also be wary of massive text blobs
of AI generated documentation given that these tend to massively bloat the size
of the contribution and are often not very useful, and thus are likely to be rejected as extractive contributions.

We expect that new contributors will be less confident in their contributions,
and our guidance to them is to **start with small contributions** that they can
fully understand to build confidence. We aspire to be a welcoming community
that helps new contributors grow their expertise, but learning involves taking
small steps, getting feedback, and iterating. Passing maintainer feedback to an
LLM doesn't help anyone grow, and does not sustain our community.

Contributors are expected to **be transparent and label contributions that
contain substantial amounts of tool-generated content**. Our policy on
labelling is intended to facilitate reviews, and not to track which parts of
RIOT are generated. Contributors should note tool usage in their pull request
description, commit message, or wherever authorship is normally indicated for
the work. For instance, use a commit message trailer like `Assisted-by: <name of
code assistant>`. This transparency helps the community develop best practices
and understand the role of these new tools.

This policy includes, but is not limited to, the following kinds of
contributions:

- Code, usually in the form of a pull request
- RFCs or design proposals
- Issues or security vulnerabilities
- Comments and feedback on pull requests
- Contributions to other repositories within the RIOT organization

## Details

To ensure sufficient self review and understanding of the work, it is
**mandatory that contributors write PR/Issue descriptions and replies themselves.**
Authors not fluent in English may write in their native language and use
a tool to translate it to English, if they also include the original text.
Use of generative AI for spell checking and grammar checking is OK, if the
text is updated by hand based on that review, rather than letting the
generative AI directly generate a revised version.

The description should explain the motivation, implementation approach, expected impact,
and any open questions or uncertainties to the same extent as a contribution made without
tool assistance. **LLM usage for communication with other humans will be**
**considered extractive under the terms of this policy and is strictly forbidden**.

An important implication of this policy is that it bans agents that take action
in our digital spaces without human approval, such as the [`@claude`
agent](https://github.com/claude/) or Copilot agent.
Similarly, automated review tools that publish comments without
human review are not allowed. However, an opt-in review tool
that **keeps a human in the loop** is acceptable under this policy.
As another example, using an LLM to generate documentation, which a contributor
manually reviews for correctness, edits, and then posts as a PR, is an approved
use of tools under this policy.

AI tools must not be used to fix GitHub issues meant for beginners as indicated
by the "Good First Issue" or "Hack'n'ACK Candidate" labels.
These issues are generally not urgent, and are
intended to be learning opportunities for new contributors to get familiar with
the codebase. Whether you are a newcomer or not, fully automating the process
of fixing these issues squanders the learning opportunity and doesn't add much
value to the project.

## Extractive Contributions

The reason for our "human-in-the-loop" contribution policy is that processing
patches, PRs, RFCs, and comments to RIOT is not free — it takes a lot of
maintainer time and energy to review those contributions! Sending the
unreviewed output of an LLM to open-source project maintainers *extracts* work
from them in the form of design and code review, so we call this kind of
contribution an "extractive contribution".

Our **golden rule** is that a contribution should be worth more to the project
than the time it takes to review it. These ideas are captured by this quote
from the book [Working in Public][public] by Nadia Eghbal:

[public]: https://press.stripe.com/working-in-public

> \"When attention is being appropriated, producers need to weigh the costs and
> benefits of the transaction. To assess whether the appropriation of attention
> is net-positive, it's useful to distinguish between *extractive* and
> *non-extractive* contributions. Extractive contributions are those where the
> marginal cost of reviewing and merging that contribution is greater than the
> marginal benefit to the project's producers. In the case of a code
> contribution, it might be a pull request that's too complex or unwieldy to
> review, given the potential upside.\" \-- Nadia Eghbal

Prior to the advent of LLMs, open-source project maintainers would often review
any and all changes sent to the project simply because posting a change for
review was a sign of interest from a potential long-term contributor. While new
tools enable more development, it shifts effort from the implementor to the
reviewer, and our policy exists to ensure that we value and do not squander
maintainer time.

Reviewing changes from new contributors is part of growing the next generation
of contributors and sustaining the project. We want RIOT to be
welcoming and open to aspiring developers who are willing to invest
time and effort to learn and grow, because growing our contributor base and
recruiting new maintainers helps sustain the project over the long term. Being
open to contributions is a big part of how RIOT has grown.
We therefore automatically post a greeting comment to pull
requests from new contributors and encourage maintainers to spend their time to
help new contributors learn.

## Handling Violations

If a maintainer judges that a contribution doesn't comply with this policy,
they should close the Pull Request with the following response:

```text
This PR doesn't appear to comply with our policy on tool-generated content,
and requires additional justification for why it is valuable enough to the
project for us to review it. Please see our developer policy on
AI-generated contributions: https://doc.riot-os.org/general/ai_policy/
You may reopen the PR after complying with this policy.
```

The best ways to make changes less extractive and more valuable is to reduce
its size or complexity or to increase its usefulness to the community. These
factors are impossible to weigh objectively, and our project policy leaves this
determination up to the maintainers of the project, i.e. those who are doing
the work of sustaining the project.

## Copyright

Artificial intelligence systems raise many questions around copyright that have
yet to be answered. Our policy on AI tools is similar to our copyright policy:
Contributors are responsible for ensuring that they have the right to
contribute code under the terms of our license, typically meaning that either
they, their employer, or their collaborators hold the copyright. Using AI tools
to regenerate copyrighted material does not remove the copyright, and
contributors are responsible for ensuring that such material does not appear in
their contributions. Contributions found to violate this policy will be removed
just like any other offending contribution.

## References

Our policy was informed by experiences in other communities:

- [Zig: Contributor Poker and AI][zig]
- [LLVM AI Policy][llvm]: As the original source of the policy above which
  we adapted for our project. [LLVM is licensed under Apache 2.0][llvm-license].
  Their policy was also inspired by the following sources:
- [Fedora Council Policy Proposal: Policy on AI-Assisted Contributions (fetched
  2025-10-01)][fedora]: Some of the text above was copied from the Fedora
  project policy proposal, which is licensed under the [Creative Commons
  Attribution 4.0 International License][cca]. This link serves as attribution.
- [Rust draft policy on burdensome PRs][rust-burdensome]
- [Seth Larson's post][security-slop]
  on slop security reports in the Python ecosystem
- The METR paper [Measuring the Impact of Early-2025 AI on Experienced
  Open-Source Developer Productivity][metr-paper].
- [QEMU bans use of AI content generators][qemu-ban]
- [Slop is the new name for unwanted AI-generated content][ai-slop]

[fedora]: https://communityblog.fedoraproject.org/council-policy-proposal-policy-on-ai-assisted-contributions/
[cca]: https://creativecommons.org/licenses/by/4.0/
[rust-burdensome]: https://github.com/rust-lang/compiler-team/issues/893
[security-slop]: https://sethmlarson.dev/slop-security-reports
[metr-paper]: https://metr.org/blog/2025-07-10-early-2025-ai-experienced-os-dev-study/
[qemu-ban]: https://www.qemu.org/docs/master/devel/code-provenance.html#use-of-ai-content-generators
[ai-slop]: https://simonwillison.net/2024/May/8/slop/
[llvm]: https://llvm.org/docs/_sources/AIToolPolicy.md.txt
[llvm-license]: https://releases.llvm.org/22.1.0/LICENSE.TXT
[zig]: https://kristoff.it/blog/contributor-poker-and-ai/
