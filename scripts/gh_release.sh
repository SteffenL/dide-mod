#!/usr/bin/env bash
set -euo pipefail

cleanup() {
    if (( ${#old_releases[@]} > 0 )); then
        echo 'Cleaning up old releases:'
        for old_tag_name in "${old_releases[@]}"; do
            echo "- ${old_tag_name}"
            if [[ "${confirm}" == 1 ]]; then
                gh release delete --yes --cleanup-tag "${old_tag_name}" || true
            fi
        done
    fi
}

confirm=0

while (( $# > 0 )); do
    arg_name=$1
    shift
    case "${arg_name}" in
        --confirm)
            confirm=1
            ;;
        *)
            echo "Invalid argument: ${arg_name}"
            exit 1
            ;;
    esac
done

today_date=$(date --utc +'%Y%m%d')
today_time=$(date --utc +'%H%M%S')
workflow_run=${GITHUB_RUN_NUMBER:-${today_date}}.${GITHUB_RUN_ATTEMPT:-${today_time}}
commit=$(git rev-parse HEAD)
short_commit=$(echo "${commit}" | cut -c-7)
tag_prefix=nightly-release-
tag_name=${tag_prefix}${workflow_run}
exit_code_release_exists=2

if [[ "${confirm}" != 1 ]]; then
    echo 'This is a read-only run. Use --confirm to apply changes.'
fi

echo "Generated tag name: ${tag_name}"

release_exists=0
old_releases=()
while IFS= read -r existing_tag_name; do
    if [[ "${existing_tag_name}" == "${tag_name}" ]]; then
        release_exists=1
        continue
    fi
    if [[ "${existing_tag_name}" != "${tag_prefix}"* ]]; then
        continue
    fi
    old_releases+=("${existing_tag_name}")
done < <(gh release list --json tagName --jq '.[].tagName')

if [[ "${release_exists}" == 1 ]]; then
        echo 'Release already exists.'
        cleanup
        exit "${exit_code_release_exists}"
fi

echo 'Creating new release.'
if [[ "${confirm}" == 1 ]]; then
    gh release create  --prerelease --draft --target "${commit}" --title 'Nightly' --notes "Nightly build made from commit ${commit}, workflow run ${workflow_run}." "${tag_name}"
fi

echo 'Uploading assets:'
while IFS= read -r file; do
    echo "- ${file}"
    if [[ "${confirm}" == 1 ]]; then
        gh release upload "${tag_name}" "${file}"
    fi
done < <(find dist/ -maxdepth 1 -type f)

echo 'Publishing.'
if [[ "${confirm}" == 1 ]]; then
    gh release edit --draft=false "${tag_name}"
fi

cleanup
