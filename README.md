# 🛡️ Identity-Aware Microservices Mesh

A high-performance distributed backend built with **C++17**, showcasing **Event-Driven Architecture (EDA)**, multi-tenant identity orchestration (OAuth 2.0 / Enterprise SSO), and efficient JWT-based authentication.

🔗 **Live Project & Telemetry**: http://65.2.37.48

---

## 🏗️ System Design & Architecture

The system follows a decoupled, event-driven architecture to handle authentication workflows and real-time processing with scalability and reliability.

- **Identity Provider (User Service)**  
  A high-performance C++ service acting as a custom Identity Provider. Supports Manual login, Google OAuth 2.0, and Okta/Auth0 integrations. Generates signed JWTs (HS256) for session management.

- **Distributed Cache (Redis)**  
  Implements a write-through caching strategy to map third-party OAuth tokens to internal sessions, significantly reducing database read load.

- **Message Broker (Apache Kafka - Aiven)**  
  Serves as the communication backbone using an event-driven model. Enables asynchronous processing and audit logging through an identity event fan-out pattern.

- **Reactive Consumer (Workflow Service)**  
  A C++ Kafka consumer that processes events and triggers downstream workflows in near real-time.

---

## 🛠️ Technical Stack

| Layer | Technology |
|------|----------|
| **Core Language** | C++17 |
| **Identity & Security** | OAuth 2.0, Okta/Auth0 (OIDC/SAML), JWT (HS256) |
| **Database & Cache** | AWS RDS (MySQL), Redis Cloud |
| **Message Broker** | Apache Kafka (Aiven) |
| **Infrastructure** | AWS EC2, Docker, Linux |

---

## 🚀 Key Engineering Highlights

- **Efficient JWT Validation**  
  Designed a stateless authentication flow using JWTs for fast and scalable request validation.

- **Real-Time System Observability**  
  Built a telemetry dashboard to monitor backend health, database latency, and Kafka activity in real time.

- **Data Consistency & Reliability**  
  Implemented a *write-then-publish* pattern to ensure events are emitted only after successful database commits.

- **Secure Infrastructure**  
  Configured SSL/TLS communication across services and secured environment variables using `.env` isolation.

---

## 📂 Repository Structure

```text
├── mesh-backend/
│   ├── identity-service/   # C++ IdP, JWT generation, OAuth routing, Redis cache
│   ├── workflow-service/   # Kafka consumer & business logic
│   └── telemetry-ui/       # Frontend for real-time observability
├── docs/                   # Architecture diagrams & logs
└── .gitignore              # Credential protection rules
```

---

## 📈 Future Roadmap

- [ ] **FinTech Integration**  
      Integrate Razorpay to simulate payment workflows and transactional event handling.

- [ ] **Session Security Enhancement**  
      Move JWT storage from client-side to secure HttpOnly cookies to improve XSS protection.

- [ ] **CI/CD Pipeline**  
      Implement GitHub Actions for automated build, testing, and Docker deployment to AWS.
