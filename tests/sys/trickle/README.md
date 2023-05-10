# Trickle Test

This test starts a trickle timer and roughly checks the diff between two
intervals to be greater than the diff of previous intervals.
After `5` callbacks, the trickle timer is reset and ends after another `7`
callbacks with either `[SUCCESS]` or `[FAILURE]`. The application exits with
`[FAILURE]` as soon as one diff is *not* greater than the previous diff.
