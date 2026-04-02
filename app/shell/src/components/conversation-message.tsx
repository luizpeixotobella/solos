import { colors } from "../styles/tokens";
import type { AgentConversationMessage } from "../types/system";

export function ConversationMessage({ message }: { message: AgentConversationMessage }) {
  return (
    <div
      style={{
        padding: 14,
        borderRadius: 14,
        background: message.tone === "accent" ? "rgba(110,86,255,0.14)" : "rgba(255,255,255,0.04)",
        color: message.tone === "accent" ? colors.text : colors.soft,
      }}
    >
      <strong style={{ textTransform: "capitalize" }}>{message.role}:</strong> {message.text}
    </div>
  );
}
