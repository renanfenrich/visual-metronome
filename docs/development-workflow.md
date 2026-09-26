# Development workflow

This repository delivers one goal or sub-goal at a time. The workflow applies
to firmware, domain, hardware, tests, documentation, and CI changes. `main`
is the stable, reviewed branch; implementation work belongs on a goal branch.

## Goal lifecycle

The lifecycle has explicit states:

```text
implementation complete -> validation complete -> ready for review -> merged -> goal complete
```

Code that compiles is only implementation complete. A goal is fully complete
only after its pull request is reviewed, CI passes, and it is squash-merged
into `main`. Start the next goal only after that merge.

## Branching

Create one branch per goal or sub-goal from the current stable `main`:

```sh
git switch main
git pull --ff-only
git switch -c feat/<goal-description>
```

Examples:

```text
feat/g6a-grouped-rhythmic-patterns
feat/g6b-eight-led-hardware
feat/g6c-accent-rendering
docs/development-workflow
fix/<short-description>
```

Do not commit feature work directly to `main`.

## Commits

Use Conventional Commits for every commit and pull-request title. Common types
in this repository are `feat:`, `fix:`, `docs:`, `test:`, `refactor:`,
`chore:`, and `ci:`.

Keep subjects imperative, lowercase after the type, concise, focused on one
logical change, and free of trailing punctuation. For example:

```text
feat: add grouped rhythmic patterns
feat: add eight-led hardware mapping
fix: preserve beat reset after signature change
test: add seven-eight grouping coverage
docs: document development workflow
refactor: separate accent rendering from sequence logic
ci: validate native tests and uno build
```

Avoid generic subjects such as `update code`, `changes`, `fix stuff`, `wip`,
or `final`.

## Local validation and readiness

Before publication, run the checks relevant to the goal. For normal
firmware/domain work, run:

```sh
platformio test -e native
platformio run -e uno
git diff --check
git status --short
```

Ready for review requires all relevant tests and the Uno build to pass, no
whitespace errors, no unexpected uncommitted files, and behavior that meets
the goal's acceptance criteria. Update documentation whenever architecture,
wiring, controls, or user-visible behavior changes. Hardware goals additionally
need an explicit physical smoke test before being called validation complete.
Do not claim that test unless it was performed.

## Publishing and pull requests

After local validation, publish the branch and open a pull request targeting
`main`:

```sh
git push -u origin <branch>
```

The PR title is a Conventional Commit because squash merge uses it for the
commit on `main`. Use this description structure:

```markdown
## Summary

- describe the main changes
- describe the behavior introduced or changed
- mention important architectural decisions

## Validation

- `platformio test -e native`: X/X passed
- `platformio run -e uno`: passed
- `git diff --check`: passed
- RAM: ...
- Flash: ...
- physical hardware smoke test: PASS, when applicable

## Architecture

- describe relevant boundaries or invariants
- state what existing behavior remains unchanged

## Deferred

- explicitly list work intentionally left for later goals
```

Verify the PR's CI checks for the published head commit. Human review and the
decision to merge are separate checkpoints. Once approved and CI is green,
squash merge the PR, then synchronize and remove the local branch:

```sh
git switch main
git pull --ff-only
git branch -d <branch>
```

## G6A example and recovery

G6A is the first example: commit `4f3b12e feat: add grouped rhythmic patterns`
introduced the domain model for accents and grouped patterns. It was validated
locally with:

```text
platformio test -e native: 41/41 passed
platformio run -e uno: passed
git diff --check: passed
RAM: 369 / 2048 bytes
Flash: 4988 / 32256 bytes
```

Its PR title is:

```text
feat: add grouped rhythmic patterns
```

If that commit exists only on local `main` while `origin/main` has not moved,
move it to its goal branch before publication:

```sh
git switch -c feat/g6a-grouped-rhythmic-patterns

git switch main
git reset --hard origin/main

git switch feat/g6a-grouped-rhythmic-patterns
git push -u origin feat/g6a-grouped-rhythmic-patterns
```

This is safe only in that stated situation: creating the branch preserves the
commit first, restoring local `main` aligns it with `origin/main`, and the
feature commit remains available on `feat/g6a-grouped-rhythmic-patterns`.
After the push, open the G6A PR and follow the lifecycle above. G6B and G6C
must wait until G6A is merged.
