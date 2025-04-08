import { Cache, State } from "swr";

// See https://swr.vercel.app/docs/advanced/cache#cache-provider
// Since getTrieForWordlist deletes old Tries, it's critical that SWR not cache them.
export function singletonCache<Data>(): Cache<Data> {
  let cache: { key: string; value: State<Data> } | undefined;

  return {
    get(key) {
      if (cache?.key === key) {
        return cache.value;
      }
      return undefined;
    },
    set(key, value) {
      cache = { key, value };
    },
    delete(key) {
      if (cache?.key === key) {
        cache = undefined;
      }
    },
    keys() {
      return (cache ? [cache.key] : []).values();
    },
  };
}
