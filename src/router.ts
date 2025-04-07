import React from "react";
import { useLocation, useNavigate } from "react-router-dom";

export function lightlyEncodeURIComponent(param: string) {
  return param.replace(/\+/g, "%2B").replace(/ /g, "+").replace(/&/g, "%26");
}

export function lightlyDecodeURIComponent(param: string) {
  return param.replace(/%26/g, "&").replace(/\+/g, " ").replace(/%2B/g, "+");
}

function createSearchParams(params?: Record<string, string>): string {
  return Object.entries(params ?? {})
    .map(([k, v]) => k + "=" + lightlyEncodeURIComponent(v))
    .join("&");
}

export type SetURLSearchParams = (
  nextInit?:
    | Record<string, string>
    | ((prev: URLSearchParams) => Record<string, string>)
) => void;

// See https://github.com/remix-run/react-router/blob/fe661c5c0405c2212a5299b75af362df9f031b11/packages/react-router-dom/index.tsx#L904
export function useLightlyEncodedSearchParams(): [
  URLSearchParams,
  SetURLSearchParams
] {
  const location = useLocation();
  const searchParams = React.useMemo(
    () => new URLSearchParams(location.search),
    [location.search]
  );
  const navigate = useNavigate();
  const setSearchParams = React.useCallback<SetURLSearchParams>(
    (nextInit) => {
      const newSearchParams = createSearchParams(
        typeof nextInit === "function" ? nextInit(searchParams) : nextInit
      );
      navigate("?" + newSearchParams, { replace: true });
    },
    [navigate, searchParams]
  );

  return [searchParams, setSearchParams];
}
