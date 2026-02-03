(function () {
  const documentationPage = document.querySelector(".information-page");

  if (documentationPage) {
    const documentationSidebar = documentationPage.querySelector(
      ".information-sidebar"
    );
    const documentationSidebarToggler = documentationPage.querySelector(
      ".information-sidebar__toggle"
    );

    if (documentationSidebarToggler) {
      documentationSidebarToggler.addEventListener("click", (e) => {
        e.preventDefault();
        documentationSidebar.classList.toggle("is-open");
      });
    }
  }
})();

function getSidebarContent() {
  return `
    <ul>
      <li class="menu-item">
        <a href="/inicio">Inicio</a>
      </li>
      <li class="menu-item menu-item-has-children">
        <a href="#">Funcionalidades</a>
        <ul>
          <li class="menu-item">
            <a href="/lista-boyas">Boyas</a>
          </li>
          <li class="menu-item">
            <a href="/lista-torretas">Torretas</a>
          </li>
        </ul>
      </li>
      <li class="menu-item menu-item-has-children">
        <a href="#">Información</a>
        <ul>
          <li class="menu-item">
            <a href="/sistema">Sistema</a>
          </li>
        </ul>
      </li>
      <li class="menu-item">
        <a href="./configuracion">Configuración</a>
      </li>
    </ul>
  `;
}

function renderSidebar() {
  const sidebarContainer = document.getElementById("sidebar-menu-content");
  if (sidebarContainer) {
    sidebarContainer.innerHTML = getSidebarContent();
  }
}

function highlightCurrentMenuItem() {
  const currentPath = decodeURIComponent(window.location.pathname);
  // console.log('Current Path:', currentPath);

  const menuItems = document.querySelectorAll(
    ".information-sidebar .menu-item a"
  );

  menuItems.forEach((item) => {
    let itemPath = item.getAttribute("href").replace(/^\.\//, "");
    // console.log('Item Path:', itemPath);

    if (currentPath.includes(itemPath)) {
      // console.log('Match found for:', itemPath);
      item.parentElement.classList.add("current-menu-item");
    } else {
      item.parentElement.classList.remove("current-menu-item");
    }
  });
}

function getUserMenuContent() {
  return `
    <div class="user-menu-wrapper">
      <button class="user-avatar-btn" id="user-avatar-btn">
        <img src="/images/users.svg" alt="User" />
      </button>
      <div class="user-dropdown" id="user-dropdown">
        <div class="user-dropdown-header">
          <span class="user-name">user</span>
          <span class="user-role">Administrator</span>
        </div>
        <div class="user-dropdown-divider"></div>
        <ul class="user-dropdown-list">
          <li><a href="#"><img src="/images/users.svg" style="height:14px; margin-right:8px; opacity:0.6;"> Account</a></li>
          <li><a href="#"><img src="/images/VitalNet-Logo.svg" style="height:14px; margin-right:8px; opacity:0.6;"> About Us</a></li>
          <li><a href="./configuracion"><img src="/images/config.svg" style="height:14px; margin-right:8px; opacity:0.6;"> Settings</a></li>
          <li class="logout"><a href="/"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#ff3b30" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="margin-right:8px; opacity:0.8;"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"></path><polyline points="16 17 21 12 16 7"></polyline><line x1="21" y1="12" x2="9" y2="12"></line></svg> Log out</a></li>
        </ul>
      </div>
    </div>
  `;
}

function renderUserMenu() {
  let menuContainer = document.getElementById("user-menu-container");
  if (!menuContainer) {
    menuContainer = document.createElement('div');
    menuContainer.id = "user-menu-container";
    menuContainer.innerHTML = getUserMenuContent();
    document.body.appendChild(menuContainer);
  }

  const avatarBtn = document.getElementById("user-avatar-btn");
  const dropdown = document.getElementById("user-dropdown");

  if (avatarBtn && dropdown) {
    avatarBtn.onclick = function (e) {
      e.stopPropagation();
      dropdown.classList.toggle("is-active");
    };

    document.addEventListener("click", function () {
      dropdown.classList.remove("is-active");
    });

    dropdown.onclick = function (e) { e.stopPropagation(); };
  }
}

document.addEventListener("DOMContentLoaded", function () {
  renderSidebar();
  renderUserMenu();
  highlightCurrentMenuItem();
});