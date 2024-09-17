export class MediaSorter {

    constructor(data, lightboModal, photographerHandler) {
        this._data = data

        this.$mainWrapper = document.querySelector('main')

        this.$wrapper = document.createElement('div')
        this.$wrapper.classList.add('form__wrapper')

        this.lightboModal = lightboModal
        this.photographerHandler = photographerHandler

    }

    sortMedias(orderBy) {

        this.clearMedias()

        let sortedData = []

        if (orderBy !== "") {

            if (orderBy === "POPULARITY") {

                sortedData = [...this._data].sort((a, b) => b.likes - a.likes)

            } else if (orderBy === "TITLE") {

                sortedData = [...this._data].sort((a, b) => a.title.toUpperCase().charCodeAt(0) - b.title.toUpperCase().charCodeAt(0))
            } else {
                sortedData = this._data
            }

            this.lightboModal.setMedias(sortedData)

            this.photographerHandler.displayGalery(sortedData, this.$mediasWrapper)
        }
    }


    clearMedias() {
        this.$mediasWrapper = document.querySelector('.media__wrapper')
        this.$mediasWrapper.innerHTML = ""
    }

    handleSelectorStyle() {
        const dropdown = document.querySelector('.sorter__dropdown');

        const select = dropdown.querySelector('.select')
        const iconOpen = dropdown.querySelector('.open-icon')
        const menu = dropdown.querySelector('.menu')
        const options = dropdown.querySelectorAll('.menu li button')
        const selected = dropdown.querySelector('.selected')

        select.addEventListener('click', () => {
            select.classList.toggle('select-clicked')

            iconOpen.classList.toggle('open-icon-rotate')

            menu.classList.toggle('menu-open')

            if (select.getAttribute('aria-expanded') === "false") {
                select.setAttribute('aria-expanded', true)
            } else {
                select.setAttribute('aria-expanded', false)
            }

        })

        options.forEach(option => {
            option.addEventListener('click', () => {

                const { innerHTML: previousSelected, id: previousSelectedId } = selected;

                selected.innerHTML = option.innerHTML;
                selected.id = option.id;

                option.innerHTML = previousSelected;
                option.id = previousSelectedId;

                select.classList.remove('select-clicked')

                iconOpen.classList.remove('open-icon-rotate')

                menu.classList.remove('menu-open')

                this.sortMedias(selected.id)
            })
        })

    }

    render() {
        const sorterSelector = `
        <div class="sorter__wrapper">
            <label for="sortMedias" id="sortMedias" tabindex="0">Trier par </label>
            <div class="sorter__dropdown" >
                <button class="select" role="button" aria-haspopup="listbox" aria-labelledby="sortMedias" aria-expanded='false' tabindex="0" aria-label="Ouvre le menu déroulant pour faire son choix de tri">
                    <span class="selected" aria-selected="true" id="DATE" role="menuitem">Date</span>
                    <i class="fas fa-chevron-left open-icon"></i>
                </button>
                <ul class="menu" role='menu'  aria-labelledby="sortMedias">
                    <li>
                        <button  tabindex="0" id="POPULARITY" role="menuitem">Popularité</button>
                    </li>
                    <li>
                        <button  tabindex="0" id="TITLE" role="menuitem">Titre</button>
                    </li>
                </ul>
            </div>
        </div>
        `
        this.$wrapper.innerHTML = sorterSelector
        this.$mainWrapper.appendChild(this.$wrapper)
        this.handleSelectorStyle()
    }
}

