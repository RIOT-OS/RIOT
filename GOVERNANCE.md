# Governance of the RIOT Community

The RIOT community is dedicated to creating a free and open source operating system for the
constrained Internet of Things [[RFC7228], [draft-ietf-iotops-7228bis]] based on open standards.
This document explains the governance of the project.

<!-- TOC start -->
<!-- TOC start, TOC end comments are required to generate TOC in Doxygen properly -->
- [Values](#values)
- [Community Processes](#community-processes)
- [Roles](#roles)
  + [Contributors](#Contributors)
  + [Maintainers](#Maintainers)
  + [Release Managers](#Release-Managers)
  + [Admins](#Admins)
  + [GitHub Owners](#GitHub-Owners)
  + [Moderators](#Moderators)
- [Decision Making](#decision-making)
- [Meetings](#meetings)
- [Code of Conduct](#code-of-conduct)
- [Security Response Team](#security-response-team)
- [Modifying this Charter](#modifying-this-charter)
- [Attribution](#attribution)
<!-- TOC end -->

## Values

The RIOT community embraces the following values:

* **Openness:** Communication and decision-making happens in the open and is discoverable for future
  reference. As much as possible, all discussions and work events eventually take place in public
  forums and open repositories. Ideas might be shared and brainstormed in confidence among
  contributors at first but there should always be a point when this process becomes open to the
  community at large.

* **Fairness:** All stakeholders have the opportunity to provide feedback and submit contributions,
  which will be considered on their merits.

* **Community over Product or Company:** Sustaining and growing our community takes priority over
  shipping code or sponsors' organizational goals. Each contributor participates in the project as
  an individual.

* **Inclusivity:** We innovate through different perspectives and skill sets, which can only be
  accomplished in a welcoming and respectful environment.

* **Participation:** Responsibilities within the project are earned through participation, and
  contributors can grow into more responsible positions.


## Community Processes
The community around RIOT gathers many IoT developers and users from around the world, from the
industry, from academia, and hobbyists. The RIOT community is open to everyone. Our channels to
communicate can be found in our [contributing guidelines]. The community self-organizes using the
roles described below.


## Roles
### Contributors
Contributors are people who contribute their work to RIOT. This includes,

- of course, code contributions, but also
- writing documentation,
- bug reports,
- other issue reports,
- reviewing PRs,
- participation in technical as well as non-technical discussions, or
- organizational considerations.

Code contributions are very welcome. In order to streamline and harmonize code quality, contributors
must follow the [contributing guidelines].

Contributors may be associated with organizations—by employment or otherwise—who have a vested
interest in RIOT or may be individuals who have their own personal stakes in RIOT. We call these
organizations and individuals “stakeholders” throughout this document to summarize them.

### Maintainers

Among contributors, some have maintainer status, which consists in rights (write access to the [RIOT
GitHub repository](https://github.com/RIOT-OS/RIOT/)) and duties. The current maintainers can be
found in the [maintainers list].

Maintainers are people who care about RIOT and want to help it grow and improve. A maintainer is not
just someone who can make changes, but someone who has demonstrated their ability to collaborate and
organize with the team, get the most knowledgeable people to review code or documentation,
contribute high-quality code or documentation, as well as follow through to fix issues (in code,
tests, or tests). More on maintaining RIOT can be found in the [maintaining guidelines].

We are constantly looking for more maintainers. So if you are up for that, please start (or
continue) contributing code and reviews!

To contact maintainers, the best is to interact over actual RIOT code on
[GitHub](https://github.com/RIOT-OS/RIOT/pulls).

#### Becoming a Maintainer

Maintainers can propose to give maintainer status to contributors that have been noticed as
particularly active in some domain of RIOT. Of course, contributors can also step forward themselves
and declare their interest to become a maintainer. In this case, maintainers should then propose
them as well, if their activities can be confirmed. The decision to grant this status is then taken
via consensus among the maintainers. If there is consensus on granting the status to a particular
contributor, a maintainer will personally contact this contributor with the proposal, which the
contributor can then accept (or turn down).

At the discretion of the proposing maintainer (typically 1 or 2 weeks after the proposal), a new
maintainer who is selected will be

- invited to the [maintainers GitHub team] by one of the admins of the RIOT project, which grants
  them the necessary GitHub rights,
- invited to the maintainer forum group by the forum moderators, which will give them access to the
  (private) maintainer part of the forum, and
- invited to the RIOT-maintainer chat room by one of the moderators of that room, for more informal
  exchanges between maintainers.

#### Removing a Maintainer

Maintainers may resign at any time if they feel that they will not be able to continue fulfilling
their project duties.

Maintainers may also be removed after being inactive, upon failure to fulfill their Maintainer
responsibilities or because of violating the Code of Conduct. This also includes actively,
persistently, and intentionally trying to harm or successfully harming the code base of RIOT.
Especially, but not limited to, endangering the security or safety of RIOT. Inactivity is defined as
a period of very low or no activity in the project. A yearly maintainer ping, an e-mail sent to
inactive maintainers, determines if the maintainer is still willing to fulfill their project duties.
During this process, the list of maintainers is reviewed. On failure to reply to the maintainer ping
within the specified amount of time (usually a month), the maintainer will be removed.

### Release Managers

Release managers make sure the quarterly release comes out in time. They are one or more maintainers
which were appointed for a specific release by the Virtual Maintainer Assembly (VMA). Their duties
include setting the dates for feature freeze for the release, enforcing the feature freeze,
coordinating the (mostly automated) tests of a release, writing the release notes and creating the
tags defining the release and its release candidates. The full set of tasks can be found in the
document [Managing a Release]. Their duties end after the release they managed is out and all
bug-fixing point releases to their release are finished.

### Admins

GitHub admins are a special subgroup among RIOT maintainers. They are marked as such in the
[maintainers list]. They have more access rights to the RIOT repositories, such as granting access
to a repository, adding new members to a team, or enabling protection for Git branches. Release
managers might need to contact GitHub admins to configure the branch protection rules for the
release branch. Beyond those technical duties and access rights, they do not have any special rights
among maintainers. They are picked by the maintainers, usually based on seniority. The maintainers
try to take care to spread the admin responsibility over several project stakeholders within the
maintainer body. This is to aspire some checks and balances between stakeholders as well as
introduce redundancies in case a stakeholder is not able to work on RIOT anymore.

There are also admins on the other RIOT discussion platforms. Beyond technical administrative duties
they do not have any special rights. These admins usually are appointed or self-appointed on merit,
i.e., whoever sets up the platform usually is (one of) its admin(s).

### GitHub Owners
Github owners are a special subgroup among RIOT GitHub admins. They are marked as such in the
[maintainers list]. Beyond this special status and the usual GitHub admin rules and duties they do
not have any special rights among maintainers.

### Moderators

Moderators are responsible to enforce the values of the RIOT community within its discussion
platforms. Each platform usually has its own set of moderators, a list of which can be found there.
The forum moderators, e.g., can be found [here](https://forum.riot-os.org/g/moderators) (link
requires you to be logged into the forum). The tools at the disposal of the moderator are also very
platform-dependent but in general, they try to resolve conflicts that may arise between
contributors, unless a [Code of Conduct] violation takes place. Moderators are people that the
community put their trust upon. As such, they are granted this status via consensus from the
community. Typically, other moderators may propose new moderators.

## Decision Making
Decisions within the RIOT community are made on the principles of “rough consensus and running code”
as [coined by the IETF](https://datatracker.ietf.org/doc/html/rfc7282). To directly quote RFC 7282
which defines the IETF governance:

>   […] our credo is that we don't let a single individual dictate
>   decisions (a king or president), nor should decisions be made by a
>   vote, nor do we want decisions to be made in a vacuum without
>   practical experience.  Instead, we strive to make our decisions by
>   the consent of all participants, though allowing for some dissent
>   (rough consensus), and to have the actual products of engineering
>   (running code) trump theoretical designs.
>
>   Having full consensus, or unanimity, would be ideal, but we don't
>   require it: Requiring full consensus allows a single intransigent
>   person who simply keeps saying "No!" to stop the process cold.  We
>   only require rough consensus: If the chair of a working group
>   determines that a technical issue brought forward by an objector has
>   been truly considered by the working group, and the working group has
>   made an informed decision that the objection has been answered or is
>   not enough of a technical problem to prevent moving forward, the
>   chair can declare that there is rough consensus to go forward, the
>   objection notwithstanding.

Within the RIOT community, the duties of an IETF working group chair fall to maintainers
knowledgeable in the area of expertise. This knowledgability is determined by their own
contributions. On decisions regarding a release, the release manager(s) take this position.

## Meetings

There are three types of regular meetings within the RIOT community:

- The annual General Assembly (GA),
- The quarterly Virtual Maintainer Assembly (VMA), and
- The Weekly Coordinational Meeting.

The GA is public and anyone who sees themselves as a member of the RIOT community can participate.
Larger community steering decisions for the community are made during the GA, e.g., electing contact
people for the Code of Conduct. The GA usually takes place during the [RIOT
Summit](https://summit.riot-os.org/), the annual get-together of the RIOT community. The GA
moderator usually is appointed by the organizers of the RIOT Summit. It is usually recorded and its
notes will be published publicly in the RIOT forum. The agenda for the GA is collected before the
assembly but may be bashed at the start of the meeting.

The Virtual Maintainer Assembly (VMA) is a closed meeting among maintainers. The VMA appoints the
release manager for upcoming releases and the moderator for the next VMA. Other maintenance
decisions such as the fate of larger sections of code are discussed after these administrative tasks
are done. The VMA usually takes place about a month after the latest release, usually in a virtual
space, such as a video conference. The VMA moderator polls the maintainers for a sufficient date
around the date of the upcoming release. Every forth VMA may or may not co-incide with the GA.
However, the VMA moderator usually decides to merge these two meetings. In this case, VMA moderator
and RIOT Summit organizers decide together on who is moderating the joint event (it may be the VMA
moderator, it may be someone else). The agenda for the VMA is collected before the assembly but may
be bashed at the start of the meeting. The notes of the VMA will be published publicly in the RIOT
forum.

The Weekly Coordinational Meeting is a closed meeting among maintainers. It usually serves as a
small communal get-together of maintainers on a regular basis. Smaller maintenance decisions are
made during these meetings, but also short term admistrative tasks are discussed. The Weekly
Coordinational Meeting usually takes place every Friday at 10:00 in a virtual space, such as a video
conference. A maintainer that feels responsible for it shares the link to the meeting as well as a
proposed agenda, which may be amended by other maintainers, usually a day in advance.

## Code of Conduct

[Code of Conduct] violations by community members will be discussed and resolved on the
conduct@riot-os.org list. If one of the appointees to that list (see the [Code of Conduct reporting
guidance] for the members on that list) is involved in a Code of Conduct violation, two forum
moderators from other stakeholders than the appointee take their place in the discussions.

## Security Response Team

The maintainers will appoint a Security Response Team to handle security reports. This committee may
simply consist of the maintainers themselves. The Security Response Team is responsible for handling
all reports of security holes and breaches according to the [security policy].

## Modifying this Charter

Changes to this Governance and its supporting documents require the approval of at least four
maintainers who all must be employed by or associated with different stakeholders.

## Attribution
This document was originally based on the [GOVERNANCE-maintainer.md template] by the Cloud Native
Computing Foundation (CNCF).

[RFC7228]: https://datatracker.ietf.org/doc/html/rfc7228
[draft-ietf-iotops-7228bis]: https://datatracker.ietf.org/doc/draft-ietf-iotops-7228bis/
[contributing guidelines]: https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md
[maintainers list]: https://riot-os.org/maintainers.html
[maintainers GitHub team]: https://github.com/orgs/RIOT-OS/teams/maintainers
[managing a release]: https://github.com/RIOT-OS/RIOT/blob/master/doc/guides/managing-a-release/README.md
[maintaining guidelines]: https://github.com/RIOT-OS/RIOT/blob/master/MAINTAINING.md
[Code of Conduct]: https://guide.riot-os.org/general/code_of_conduct/
[Code of Conduct reporting guidance]: https://guide.riot-os.org/general/code_of_conduct/reporting/
[security policy]: https://github.com/RIOT-OS/RIOT/blob/master/SECURITY.md
[GOVERNANCE-maintainer.md template]: https://github.com/cncf/project-template/blob/main/GOVERNANCE-maintainer.md
