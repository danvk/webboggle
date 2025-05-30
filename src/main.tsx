import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { BrowserRouter } from "react-router-dom";
import { SWRConfig } from "swr";
import App from "./App.tsx";
import { singletonCache } from "./cache.ts";

import "./index.css";

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <BrowserRouter>
      <SWRConfig value={{ provider: singletonCache }}>
        <App />
      </SWRConfig>
    </BrowserRouter>
  </StrictMode>
);
