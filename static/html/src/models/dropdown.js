export class Dropdown {
    constructor(data, search) {
        this.search = search
        this._id = data.id
        this._name = data.name
        this._class = data.class

        this.$dropdown = document.createElement('div')
        this.$dropdown.classList.add('dropdown__content', this._class ?? this._class)

        this._query = ""

        this.$dropdownWrapper = document.querySelector('.dropdowns-container')
        this.$tagWrapper = document.querySelector('.tags-container')

        this.create()
    }

    create() {
        const content = `
            <input type="button" value="${this._name}" id="${this._id}" id="searchDropdown">
            <i class="fas fa-chevron-up chevron_down"></i>
            <ul class="dropdown__list hidden ${this._class || ""}">
            </ul>
            `
        this.$dropdown.innerHTML = content
        this.$dropdownWrapper.appendChild(this.$dropdown)
        this.fillList()
        this.handleDropdown()
        this.onSearch()
    }

    update() {
        this.fillList()
    }

    fillList() {
        const tags = this.search.tags[this._id]
        const selectedTags = this.search.selectedTags[this._id]

        let list = tags.filter(tag => !selectedTags?.includes(tag))

        if (this._query) {
            list = list.filter(tag => tag.toLowerCase().includes(this._query.toLowerCase()))
        }
        this.displayList(list)
    }

    displayList(list) {
        const ul = this.$dropdown.querySelector('ul')
        ul.innerHTML = ''

        list.map(tag => {
            const li = document.createElement('li')
            li.innerHTML = tag
            this.onSelectTag(li, tag)
            return li
        }).forEach(tag => ul.appendChild(tag))
    }

    onSelectTag(element) {
        element.addEventListener('click', (e) => {
            const elementSelected = e.target.innerHTML
            const { value } = this.$dropdown.querySelector('input')
            this.createTag(elementSelected)
            this.fillList()
            this.search.update()
        })
    }

    saveTag(tag) {
        this.search.addTag({
            id: this._id,
            tag
        })
        this.fillList()
    }

    createTag(tag) {
        const btn = document.createElement('button')
        btn.classList.add("tag", this._class)

        const content = `
                <span>${tag}</span>
                <i class="far fa-times-circle"></i>
            `
        btn.innerHTML = content

        this.$tagWrapper.appendChild(btn)
        this.saveTag(tag)
        this.onDeleteTag(btn, tag)
    }

    onDeleteTag(btn, tag) {
        btn.addEventListener('click', () => {
            this.search.removeTag({
                id: this._id,
                tag
            })
            const { value } = this.$dropdown.querySelector('input')
            if (value) {
                this.fillList(value)
            } else {
                this.fillList()
            }
            this.search.update()
            btn.remove()
        })
    }

    onSearch() {
        const input = this.$dropdown.querySelector('input')

        input.addEventListener('input', (e) => {

            const { value } = e.target

            this._query = value
            this.fillList()
        })

    }

    onInput(value) {

        const ul = this.$dropdown.querySelector('ul')

        ul.innerHTML = "";
        [...this.recipes]
            .filter(recipe => recipe.toLowerCase().includes(value.toLowerCase()) && !this.selectedTags[this.section.toLowerCase()]?.includes(recipe))
            .map(el => {
                const li = document.createElement('li')
                li.innerHTML = el
                this.onSelectTag(li, value)
                return li
            }).forEach(r => ul.appendChild(r))
    }

    handleDropdown() {

        const dropdown = this.$dropdownWrapper.querySelector(`#${this._id}`).parentNode
        const list = dropdown.querySelector('.dropdown__list')
        const input = dropdown.querySelector('input')
        const chevron = dropdown.querySelector('.fa-chevron-up')

        dropdown.addEventListener('click', (e) => {

            const chevronTarget = e.target.classList.contains('fa-chevron-up')

            if (dropdown.classList.contains('dropdown__content-active') && chevronTarget) {
                dropdown.classList.remove('dropdown__content-active')
                list.classList.add('hidden')
                chevron.classList.add('chevron_down')
                input.setAttribute('type', 'button')
                input.setAttribute('value', this._name)
            } else {
                dropdown.classList.add('dropdown__content-active')
                list.classList.remove('hidden')
                chevron.classList.remove('chevron_down')
                input.setAttribute('type', 'input')
                input.removeAttribute('value', '')
                input.setAttribute('placeholder', "Rechercher des " + this._name)
            }
        })
    }
}