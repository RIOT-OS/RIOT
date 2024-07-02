#!/usr/bin/env python3
"""
Command line utility to get the image sha256 sum and the manifest sha256 sum
of the latest image at dockerhub.
"""
import http.client
import json
import sys


def get_docker_token(repo):
    """
    Get an API access token for the docker registry

    :param repo: the repository the API token should be valid for
    :type repo: str
    :return: the access token to use
    :rtype: str
    """
    conn = http.client.HTTPSConnection("auth.docker.io")
    conn.request("GET",
                 f"/token?service=registry.docker.io&scope=repository:{repo}:pull",
                 headers={"Accept": "*/*"})
    resp = conn.getresponse()
    if resp.status != 200:
        raise Exception(f"Tried to get a docker token, but auth.docker.io "
                        f"replied with {resp.status} {resp.reason}")
    decoded = json.loads(resp.read())
    conn.close()
    return decoded["token"]


def get_manifest(repo, tag="latest", token=None):
    """
    Get the manifest of the given repo

    :param repo: The repository to get the latest manifest of
    :type repo: str
    :param tag: The tag to get the manifest of. (Default: "latest")
    :type tag: str
    :param token: The authorization token to use for the given repo.
                  (Default: get a fresh one.)
    :type token: str
    :return: the parsed manifast
    :rtype: dict
    """
    token = get_docker_token(repo) if token is None else token
    conn = http.client.HTTPSConnection("index.docker.io")
    hdrs = {
        "Accept": "application/vnd.docker.distribution.manifest.v2+json",
        "Authorization": f"Bearer {token}"
    }
    conn.request("GET", f"/v2/{repo}/manifests/{tag}", headers=hdrs)
    resp = conn.getresponse()
    if resp.status != 200:
        raise Exception(f"Tried to get a docker manifest, but "
                        f"index.docker.io replied with {resp.status} "
                        f"{resp.reason}")
    repo_digest = resp.getheader("ETag")[len("\"sha256:"):-1]
    decoded = json.loads(resp.read())
    conn.close()
    return (decoded, repo_digest)


def get_upstream_digests(repo, tag="latest", token=None):
    """
    Get the SHA256 hash of the latest image of the given repo at dockerhub
    as string of hex digests

    :param repo: The repository to get the hash from
    :type repo: str
    :param tag: The tag to get the manifest of. (Default: "latest")
    :type tag: str
    :param token: The authorization token to use for the given repo.
                  (Default: get a fresh one.)
    :type token: str
    :return: A 2-tuple of the image digest and the repo digest
    :rtype: (str, str)
    """
    token = get_docker_token(repo) if token is None else token
    manifest, repo_digest = get_manifest(repo, tag=tag, token=token)
    digest = manifest["config"]["digest"]
    return (digest[len("sha256:"):], repo_digest)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(f"Usage {sys.argv[0]} <REPO_NAME>")

    digest, repo_digest = get_upstream_digests(sys.argv[1])
    print(f"{digest} {repo_digest}")
