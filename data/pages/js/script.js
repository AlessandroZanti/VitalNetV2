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

document.addEventListener("DOMContentLoaded", function () {
  renderSidebar();
  highlightCurrentMenuItem();
});