#!/bin/sh
# This tools checks if the makefiles/features_existing.inc.mk is up to date
hash_feature_list_pre="$(sha256sum "$(dirname "$0")/../../../makefiles/features_existing.inc.mk")"
make --silent -C "$(dirname "$0")"/../../.. generate-features || exit 1
hash_feature_list_post="$(sha256sum "$(dirname "$0")/../../../makefiles/features_existing.inc.mk")"
if [ "$hash_feature_list_pre" != "$hash_feature_list_post" ]; then
    echo "Forgot to run make generate-features after updating features.yaml!"
    exit 1
fi

exit 0
