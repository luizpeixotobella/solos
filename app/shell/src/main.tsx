import React from "react";
import ReactDOM from "react-dom/client";
import { ShellRoot } from "./app/shell-root";
import "./styles/global.css";

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <ShellRoot />
  </React.StrictMode>,
);
